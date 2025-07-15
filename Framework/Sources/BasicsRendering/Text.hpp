#pragma once

#include "Common.hpp"
#include <ft2build.h>
#include FT_FREETYPE_H



//////////////////////////// 
/*
General pipeline:
The objective is to construct the class with the font name and the size at the start.
It automatically creates a texture atlas looping through storeGlyph in createAtlasTexture, storing the metrics

Considerations:
A lot of variables should be cleared after the createAtlasTexture creation. These variables should be inside functions and not as class variables
It only allows one font type and size per atlas, which I don't know if it positive or negative
No kerning, it can improve spacing between glyphs for specific pairs, like AA vs AV

*/

/////////////////////////////////////////////////////////////////////////////////////////////////
	//Falta poner texto estático, dinámico y multiples inputs en text to draw, texto en dpis, reserves
	/////////////////////////////////////////////////////////////////////////////////////////////////

struct Line {
	p2 pos;
	string text;

	template <typename... Args>
	Line(p2 pos_, Args&&... body) : pos(pos_)
	{
		std::ostringstream oss;
		(oss << ... << body);
		text = oss.str();
	}
};


struct Text {

	unsigned int vertexArray, vertexBuffer, indexBuffer, textAtlasTexture;

	vector<float> positions;	//quads
	vector<unsigned int> indices;

	FT_Library ft;
	//A typeface (or font family) englobes all the stylings of a font, a typefont is Helvetica while a font is Helvetica Regular
	FT_Face face; //a face is the structure that stores all the data for a specific font

	string glyphPath;
	int fontPixelSize;

	string allGlyphs; //a font is a collection of glyphs
	bool isBufferUpdated = true;

	//initializes the library and loads the font face
	void initializeFreeType(const std::string& fontPath, const int fontPixelSize);


	//in oneNote you have images of what every metric is.
	struct GlyphMetrics {
		float width, height;
		float bearingX, bearingY;
		float advance;
		float texCoordX0 = 0, texCoordY0 = 0;
		float texCoordX1 = 1, texCoordY1 = 1;
	};
	std::map<char, GlyphMetrics> glyphMetricsMap; //will store all the metrics





	Text(string glyphPath_, int fontPixelSize_) :glyphPath(glyphPath_), fontPixelSize(fontPixelSize_) {

		initializeFreeType(glyphPath, fontPixelSize);
		initializeBuffer();
		createAtlasTexture();


	}






	vector<p2> textPosition;
	vector<string> textToDraw;



	
	//Hazte un resumen comentario de en que formato se llama cada función, no?
	////Currently if you use substituteText in a while loop it will be equivalent to using addDynamicText, that can't be right

	//meant to be the initial push for static text. It wont delete previous entries. Accepts vector a single Line format
	void addText(vector<Line> line)
	{
		for (auto& l : line)
		{
			textPosition.push_back(l.pos);
			textToDraw.push_back(l.text);
		}

		fillVertexBuffer();
	}
	//line must go inside {} in the call, in the vector call and Dybamic the format is: {{},{}};
	void addText(Line line)
	{

		textPosition.push_back(line.pos);
		textToDraw.push_back(line.text);


		fillVertexBuffer();
		isBufferUpdated = true;

	}

	//meant to substitute a single entry
	void substituteText(unsigned int i, Line line)
	{

		textPosition[i] = line.pos;
		textToDraw[i] = line.text;


		fillVertexBuffer();
		isBufferUpdated = true;
	}
	//fancy function if you don't want to change the position. Here you call it with (index, oss) instead of (index,{p2,oss})
	template <typename... Args>
	void substituteText(unsigned int i, Args&&... body)
	{
		std::ostringstream oss;
		(oss << ... << body);

		textToDraw[i] = oss.str();

		fillVertexBuffer();
		isBufferUpdated = true;

	}

	void addDynamicText(vector<Line> line)
	{
		textPosition.clear();
		textToDraw.clear();

		for (auto& l : line)
		{
			textPosition.push_back(l.pos);
			textToDraw.push_back(l.text);
		}

		fillVertexBuffer();
		isBufferUpdated = true;

	}













	//Creates and sets the texture of the atlas first without data
	void initializeAtlasTexture(const float atlasWidth, const float atlasHeight);


	//Stores the metrics of a character inside a map and the sum of its advances gets the width of the atlas
	void storeGlyph(char character, float& atlasWidth);

	//main function that includes the initialization of the texture and the call of storeGlyph to end with the final Atlas
	void createAtlasTexture();




	//fills the vertex buffer with the final quad positions and with the coordinates of the glyph in the atlas
	void fillVertexBuffer();

	int indexOffset = 0; //to separate different textToDraws
	void createIndices(size_t i);


	void initializeBuffer();






	void draw() {
		//if (isBufferUpdated) 
		{
			glBindVertexArray(vertexArray);
			glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
			glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(float), /*positions.data()*/ nullptr, GL_DYNAMIC_DRAW);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * 4, indices.data(), GL_DYNAMIC_DRAW);

			glBindTexture(GL_TEXTURE_2D, textAtlasTexture);
			glBufferSubData(GL_ARRAY_BUFFER, 0, positions.size() * 4, positions.data());

			isBufferUpdated = false;
		}
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, nullptr);
	}


	~Text();

};

template<typename T>
T round2d(T number) {
	return round(number * 100.0) / 100.0;
}

template<typename T>
T round1d(T number) {
	return round(number * 10.0) / 10.0;
}