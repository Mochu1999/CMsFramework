#include "Text.hpp"

//initializes the library and loads the font face
void Text::initializeFreeType(const std::string& fontPath, const int fontPixelSize) {
	if (FT_Init_FreeType(&ft)) {
		std::cerr << "Could not init FreeType Library" << std::endl;
		return;
	}

	if (FT_New_Face(ft, fontPath.c_str(), 0, &face)) {
		std::cerr << "Failed to load font" << std::endl;
		return;
	}
	// Set font size in pixels. Word and other apps set the size in dots per inch. RESCALING IS NOT AN OPTION YET
	FT_Set_Pixel_Sizes(face, 0, fontPixelSize); 

	FT_ULong  charcode; //unicode codepoint (the number that represents the character)
	FT_UInt   glyphIndex;

	
	charcode = FT_Get_First_Char(face, &glyphIndex); //it's 32, if you force glyphIndex to be 0 things as ñ appear in allGlyphs (but there's no glyph for it, idk)

	while (glyphIndex != 0) {
		//128 first characters are ascii characters. For latin-1 characters (ñ, á, ü) you should have the next 128 or maybe just delete the if
		if (charcode < 256) 
		{
			allGlyphs.push_back(static_cast<char>(charcode));
		}
		
		charcode = FT_Get_Next_Char(face, charcode, &glyphIndex);
	}

	//std::cout << "Extracted glyphs: " << allGlyphs << std::endl;
}


void Text::initializeBuffer() {
	glBindVertexArray(0);
	glGenVertexArrays(1, &vertexArray);
	glBindVertexArray(vertexArray);

	glGenBuffers(1, &vertexBuffer);
	glGenBuffers(1, &indexBuffer);

	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);


	//First 2 floats of the vertex buffer are the position and the next 2 the texture coordinates
	//offset is the last element: 2 floats; stride is the next to last element: 4 floats

	// Position attribute
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

	// text coordinate attribute
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

	glBindVertexArray(0);
}

//Creates and sets the texture of the atlas first without data
void Text::initializeAtlasTexture(const float atlasWidth, const float atlasHeight) {
	glGenTextures(1, &textAtlasTexture);
	glBindTexture(GL_TEXTURE_2D, textAtlasTexture);



	glTexImage2D(
		GL_TEXTURE_2D,
		0,//level of detail, setted on base detail
		GL_RED,	//this gives the internal format, look for more info
		atlasWidth, // width of the entire texture atlas.
		atlasHeight, //height of the tallest glyph in the atlas, used as the atlas height.
		0, //border of the texture, must be 0
		GL_RED,
		GL_UNSIGNED_BYTE,
		nullptr);



	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); //linearly interpolates the pixel, smoother, but somewhat blurry 
	/*glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);*/ //choose the nearest pixel: pixelated, but sharp 
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);//clamps texture outside 0,1 range? Don't think I need it

	glBindTexture(GL_TEXTURE_2D, 0);
}

//Stores the metrics of a character inside a map
void Text::storeGlyph(char character, float& widthAtlas) {

	if (FT_Load_Char(face, character, FT_LOAD_RENDER))
	{
		std::cout << "Failed to load glyph" << std::endl;
	}

	FT_GlyphSlot& glyph = face->glyph;
	FT_Bitmap& bitmap = glyph->bitmap; //A bitmap is the FreeType structure that contains the data of a glyph

	//Glyph metrics
	float bearingX = glyph->bitmap_left;
	float bearingY = glyph->bitmap_top;
	float width = bitmap.width;
	float height = bitmap.rows;
	float advance = glyph->advance.x >> 6;


	widthAtlas += width;


	glyphMetricsMap.emplace(character, GlyphMetrics{ width, height, bearingX, bearingY, advance });

}

//main function that includes the initialization of the texture and the call of storeGlyph to end with the final Atlas
void Text::createAtlasTexture() {

	float atlasWidth = 0, atlasHeight = 0;//Total dimensions of the atlas

	for (char& c : allGlyphs)
	{

		storeGlyph(c, atlasWidth); //creates metrics map and sets atlas dimensions
		if (glyphMetricsMap[c].height > atlasHeight)
		{
			atlasHeight = glyphMetricsMap[c].height; //highest glyph determines final atlasHeight
		}
	}

	initializeAtlasTexture(atlasWidth, atlasHeight);

	//OpenGL by default expects the rows of data to be packed in multiples of 4 bytes, it isn't our case, putting it in 1 somehow means 
	// that “Each row is packed with no extra padding—just one byte per pixel, back-to-back.” and it just works without removing efficiency
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	float currentWidth = 0; //sets the x position of the glyph in the atlas
	for (char& c : allGlyphs) { //stores a texture of each glyph in the atlas

		glBindTexture(GL_TEXTURE_2D, textAtlasTexture);//necessary here

		if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
			std::cout << "Failed to load glyph " << c << std::endl;
			continue;
		}


		//cout << "atlasWidth: " << atlasWidth << " atlasHeight: " << atlasHeight << endl;
		//cout << "face->glyph->bitmap.width: " << face->glyph->bitmap.width << " face->glyph->bitmap.rows: " << face->glyph->bitmap.rows << endl;
		//cout << "currentWidth: " << currentWidth << endl;

		FT_Bitmap& bitmap = face->glyph->bitmap; //A bitmap is the FreeType structure that contains the data of a glyph
		glTexSubImage2D(
			GL_TEXTURE_2D,
			0, //level of detail, setted on base detail
			currentWidth, //X offset in the texture where the subimage will be placed
			0, //Y offset in the texture
			bitmap.width, // width of the subimage (glyph)
			bitmap.rows, //  height of the subimage
			GL_RED,
			GL_UNSIGNED_BYTE,
			bitmap.buffer // The actual bitmap data for the glyph.
		);


		//height is the total height, width is the total width of the glyph, advance is the width plus the space till next glyph
		glyphMetricsMap[c].texCoordX0 = currentWidth / atlasWidth;
		glyphMetricsMap[c].texCoordX1 = (currentWidth + glyphMetricsMap[c].width) / atlasWidth;
		glyphMetricsMap[c].texCoordY0 = 0;
		glyphMetricsMap[c].texCoordY1 = glyphMetricsMap[c].height / atlasHeight;

		currentWidth += glyphMetricsMap[c].width;


	}
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4); //Returning to default for next operations

	/*for (auto& pair : glyphMetricsMap) {

		char i = pair.first;
		cout << "Glyph: " << i << endl;
		print(glyphMetricsMap[i].width);
		print(glyphMetricsMap[i].height);
		print(glyphMetricsMap[i].bearingX);
		print(glyphMetricsMap[i].bearingY);
		print(glyphMetricsMap[i].advance);
		print(glyphMetricsMap[i].texCoordX0);
		print(glyphMetricsMap[i].texCoordY0);
		print(glyphMetricsMap[i].texCoordX1);
		print(glyphMetricsMap[i].texCoordY1);
		cout << endl << endl;
	}*/

	glBindTexture(GL_TEXTURE_2D, 0);

	//glyphMetricsMap.clear(); //Clears that memory bc it's not going to be used anymore
}



//fills the vertex buffer with the final quad positions and the atlas coordinates of the glyph
void Text::fillVertexBuffer() {
	positions.clear();
	indices.clear();
	indexOffset = 0;

	
	for (size_t i = 0; i <  textToDraw.size(); i++)
	{

		//bottom left coordinates of the string to render
		int x =  textPosition[i].x;
		int y =  textPosition[i].y;

		for (size_t j = 0; j <  textToDraw[i].size(); ++j) {

			char c =  textToDraw[i][j];
			GlyphMetrics metrics = glyphMetricsMap[c];

			//logic is sound
			float x0 = x + metrics.bearingX;
			float y0 = y - metrics.height + metrics.bearingY;
			float x1 = x0 + metrics.width;
			float y1 = y0 + metrics.height;


			float s0 = metrics.texCoordX0;
			float t0 = metrics.texCoordY0;
			float s1 = metrics.texCoordX1;
			float t1 = metrics.texCoordY1;


			positions.insert(positions.end(), //positions and texture coordinates interleaved
				{ x0, y0, s0, t1,
				x1, y0, s1, t1,
				x1, y1, s1, t0,
				x0, y1, s0, t0
				});


			x += metrics.advance;

			createIndices(j);
		}
			indexOffset = indices.back() + 1;
	}
}

//2 triangles per each quad in the order {0,1,2 , 0,2,3}
void Text::createIndices(size_t i) {
	unsigned int aux = i * 4;
	indices.insert(indices.end(), { indexOffset + aux,indexOffset + aux + 1,indexOffset + aux + 2,indexOffset + aux,indexOffset + aux + 2,indexOffset + aux + 3 });
}




Text::~Text() {
	// Clean up the resources
	glDeleteTextures(1, &textAtlasTexture);
	glDeleteBuffers(1, &vertexBuffer);
	glDeleteBuffers(1, &indexBuffer); // Ensure you've created and stored the indexBuffer handle
	glDeleteVertexArrays(1, &vertexArray);

	// Cleanup FreeType resources
	FT_Done_Face(face);
	FT_Done_FreeType(ft);
}