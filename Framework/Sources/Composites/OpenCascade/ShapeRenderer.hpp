#include "AuxFunctionsOC.hpp"


#pragma once



struct ShapeRenderer
{
	TopoDS_Shape shape;

	GlobalVariables& gv;
	Shader& shader3D;

	unsigned int trVAO, VBO, trIBO, trNBO; //shared vertex buffer between tr and wire
	unsigned int wireVAO, wireIBO;

	std::vector<p3> positions, trNormals;
	std::vector<unsigned int> trIndices, wireIndices;


	ShapeRenderer(GlobalVariables& gv_, Shader& shader3D_) :gv(gv_), shader3D(shader3D_)
	{
		genBuffers();

	}

	//XZ plane
	void addRectangle(p3 corner1, p3 corner2)
	{
		clear();
		createRectangle(corner1, corner2, { 0,1,0 }, shape);


		extractShapeData();
		updateBuffers();
	}
	void addPolyline(vector<p3> positions)
	{
		clear();

		createPolyline(positions, shape);

		extractShapeData();
		updateBuffers();
	}
	void addCircle(const p3& center, const float radius, p3 normal = {0,1,0})
	{
		clear();

		createCircle(center, radius, shape, normal);

		extractShapeData();
		updateBuffers();
	}

	void extrudeFace(float length)
	{
		extrudeSingleFace(shape, { 0,length,0 });

		extractShapeData();
		updateBuffers();
	}


	void addIGES(const string iges)
	{
		clear();

		importIGES(iges, shape);

		extractShapeData();

		updateBuffers();
	}

	void addBoxShape(p3 corner, p3 size)
	{
		clear();

		gp_Pnt gpCorner(corner.x, corner.y, corner.z);
		shape = BRepPrimAPI_MakeBox(
			gpCorner,
			static_cast<Standard_Real>(size.x),
			static_cast<Standard_Real>(size.y),
			static_cast<Standard_Real>(size.z)
		);

		extractShapeData();
		updateBuffers();
	}

	void addSphereShape(p3 center, float radius)
	{
		clear();

		gp_Pnt gpCenter(center.x, center.y, center.z);
		shape = BRepPrimAPI_MakeSphere(gpCenter, static_cast<Standard_Real>(radius));

		extractShapeData();

		trNormals.clear();

		for (auto& pos : positions)
		{
			trNormals.push_back(normalize3(pos - center));
		}

		updateBuffers();
	}

	void addShape(TopoDS_Shape& shape_)
	{
		clear();
		shape = shape_;
		extractShapeData();
		updateBuffers();
	}


	void genBuffers()
	{
		glGenVertexArrays(1, &trVAO);
		glGenVertexArrays(1, &wireVAO);

		glGenBuffers(1, &VBO);
		glGenBuffers(1, &trIBO);
		glGenBuffers(1, &wireIBO);
		glGenBuffers(1, &trNBO);

		glBindVertexArray(trVAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glBindBuffer(GL_ARRAY_BUFFER, trNBO);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glBindVertexArray(0);


		glBindVertexArray(wireVAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0); //wireVAO must also be aware of the layout

		glBindVertexArray(0);
	}

	//Extracts the rendering data of the shape
	void extractShapeData()
	{
		int counter = 0;
		//Iterating through each face to get the rendering data
		for (TopExp_Explorer exp(shape, TopAbs_FACE); exp.More(); exp.Next())
		{
			const TopoDS_Face& face = TopoDS::Face(exp.Current());

			extractFaceData(face, positions, trIndices, wireIndices, trNormals);
			//counter++;
			//print(counter);
		}
	}

	void clear()
	{
		shape.Nullify();

		positions.clear();
		trIndices.clear();
		wireIndices.clear();
		trNormals.clear();
	}

	~ShapeRenderer()
	{
		glDeleteVertexArrays(1, &trVAO);
		glDeleteBuffers(1, &VBO);
		glDeleteBuffers(1, &trIBO);
		glDeleteBuffers(1, &trNBO);

		glDeleteVertexArrays(1, &wireVAO);
		glDeleteBuffers(1, &wireIBO);
	}


	//only at the start and after the data is modified
	//I am assuming the data does not usually change so I am not using subdata
	void updateBuffers()
	{
		glBindVertexArray(trVAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(p3), positions.data(), GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, trIBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, trIndices.size() * sizeof(unsigned int), trIndices.data(), GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, trNBO);
		glBufferData(GL_ARRAY_BUFFER, trNormals.size() * sizeof(p3), trNormals.data(), GL_STATIC_DRAW);

		glBindVertexArray(wireVAO);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, wireIBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, wireIndices.size() * sizeof(unsigned int), wireIndices.data(), GL_STATIC_DRAW);
	}


	void draw(p3 color = {1,1,1}) const
	{

		shader3D.setUniform("u_Color", color.x, color.y, color.z, 1);

		if (gv.visualizationMode == triangulated)
		{
			shader3D.setUniform("u_fragmentMode", 0);

			glBindVertexArray(trVAO);
			glDrawElements(GL_TRIANGLES, trIndices.size(), GL_UNSIGNED_INT, nullptr);

			glBindVertexArray(wireVAO);
			glDrawElements(GL_LINES, wireIndices.size(), GL_UNSIGNED_INT, nullptr);
		}
		else //(gv.visualizationMode == wire)
		{
			shader3D.setUniform("u_fragmentMode", 1);

			glBindVertexArray(wireVAO);
			glLineWidth(1);
			glDrawElements(GL_LINES, wireIndices.size(), GL_UNSIGNED_INT, nullptr);
			glLineWidth(1);
		}
		glBindVertexArray(0);
	}


	void writeDataOpenCascade() 
	{

		std::string path = "Resources/OpenCascade/model.bin";

		std::ofstream outFile(path, std::ios::binary);
		if (outFile)
		{
			//positions
			size_t size = positions.size();
			outFile.write(reinterpret_cast<const char*>(&size), sizeof(size));
			outFile.write(reinterpret_cast<const char*>(positions.data()), size * sizeof(p3));

			//normals
			size = trNormals.size();
			outFile.write(reinterpret_cast<const char*>(&size), sizeof(size));
			outFile.write(reinterpret_cast<const char*>(trNormals.data()), size * sizeof(p3));

			//trIndices
			size = trIndices.size();
			outFile.write(reinterpret_cast<const char*>(&size), sizeof(size));
			outFile.write(reinterpret_cast<const char*>(trIndices.data()), size * sizeof(unsigned int));

			//wireIndices
			size = wireIndices.size();
			outFile.write(reinterpret_cast<const char*>(&size), sizeof(size));
			outFile.write(reinterpret_cast<const char*>(wireIndices.data()), size * sizeof(unsigned int));

			outFile.close();
		}
		else
		{
			std::cerr << "Error opening file for writing." << std::endl;
		}
		
	}
	void readDataOpenCascade() 
	{

		std::string path = "Resources/OpenCascade/model.bin";

		std::ifstream inFile(path, std::ios::binary);
		if (inFile)
		{
			size_t size;

			inFile.read(reinterpret_cast<char*>(&size), sizeof(size));
			positions.resize(size);
			inFile.read(reinterpret_cast<char*>(positions.data()), size * sizeof(p3));

			inFile.read(reinterpret_cast<char*>(&size), sizeof(size));
			trNormals.resize(size);
			inFile.read(reinterpret_cast<char*>(trNormals.data()), size * sizeof(p3));

			inFile.read(reinterpret_cast<char*>(&size), sizeof(size));
			trIndices.resize(size);
			inFile.read(reinterpret_cast<char*>(trIndices.data()), size * sizeof(unsigned int));

			inFile.read(reinterpret_cast<char*>(&size), sizeof(size));
			wireIndices.resize(size);
			inFile.read(reinterpret_cast<char*>(wireIndices.data()), size * sizeof(unsigned int));

			inFile.close();
		}
		else
		{
			std::cerr << "Error opening file for reading." << std::endl;
		}
		inFile.close();
	}

	void saveShape()
	{
		BinTools::Write(shape, "Resources/OpenCascade/model.brep");
		writeDataOpenCascade();
		print("saved");
	}
	void openShape()
	{

		print("reading Opencascade");

		BinTools::Read(shape, "Resources/OpenCascade/model.brep");
		print("extracting Data");

		readDataOpenCascade();

		updateBuffers();

		print("opened");
	}
	void importStlOpenCascade()
	{
		clear();

		StlAPI_Reader reader;
		print("reading Opencascade");
		reader.Read(shape, "Resources/OpenCascade/tust.stl");
		print("extracting Data");
		extractShapeData();


		trNormals.clear();
		std::ifstream inFile("Resources/OpenCascade/tust.stl");

		if (!inFile) {
			std::cerr << "Unable to open file: "  << std::endl;
			return;
		}
		std::string line;
		p3 normal, vertex;
		print("calculating normals");
		while (std::getline(inFile, line)) 
		{
			std::istringstream iss(line);
			std::string word;
			iss >> word;

			if (word == "facet") 
			{
				// Read normal vector
				iss >> word; // Skip "normal"
				iss >> normal.x >> normal.y >> normal.z;
				trNormals.insert(trNormals.end(), { normal ,normal ,normal });
			}
		}
		inFile.close();

		updateBuffers();
	}





	
};




