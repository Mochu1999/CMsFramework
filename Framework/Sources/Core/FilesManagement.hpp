#pragma once

#include <filesystem>
namespace fs = std::filesystem;
#include "Common.hpp"
#include "Polyhedra.hpp"

//A lo mejor meter esto en binaries manager. En la versión 2d queda más bonito

void writeModel(const std::vector<p3>& model) {
	std::string basePath = "Resources/Plain models/";
	std::string modelPath;

	std::cout << "Current models: " << std::endl;
	for (const auto& entry : fs::directory_iterator(basePath)) {
		std::cout << "     " << entry.path().filename() << std::endl;
	}
	std::cout << "Enter model name: ";
	std::cin >> modelPath;
	std::string path = basePath + modelPath;
	std::cout << "Setting model in: " << path << std::endl;

	std::ofstream outFile(path, std::ios::binary);
	if (outFile) {
		size_t size = model.size();
		outFile.write(reinterpret_cast<const char*>(&size), sizeof(size));

		// Write each point in the model
		outFile.write(reinterpret_cast<const char*>(model.data()), size * sizeof(p3));
	}
	else {
		std::cerr << "Error opening file for writing." << std::endl;
	}
	outFile.close();
}



// Function to read a model from a binary file
std::vector<p3> readModel() {
	std::string basePath = "Resources/Plain models/";
	std::string modelPath;
	std::cout << "set model path to one of these: " << std::endl;
	for (const auto& entry : fs::directory_iterator(basePath)) {
		std::cout << "     " << entry.path().filename() << std::endl;
	}
	std::cout << "Enter model name: ";
	std::cin >> modelPath;
	std::vector<p3> model;
	std::string path = basePath + modelPath;

	std::ifstream inFile(path, std::ios::binary);
	if (inFile) {
		size_t size;
		inFile.read(reinterpret_cast<char*>(&size), sizeof(size));
		model.resize(size);
		inFile.read(reinterpret_cast<char*>(model.data()), size * sizeof(p3));
	}
	else {
		std::cerr << "Error opening file for reading." << std::endl;
	}
	inFile.close();

	return model;
}

std::vector<p3> readModel(const std::string modelPath) {
	std::string basePath = "Resources/Plain models/";

	std::vector<p3> model;
	std::string path = basePath + modelPath;

	std::ifstream inFile(path, std::ios::binary);
	if (inFile) {
		size_t size;
		inFile.read(reinterpret_cast<char*>(&size), sizeof(size));
		model.resize(size);
		inFile.read(reinterpret_cast<char*>(model.data()), size * sizeof(p3));
	}
	else {
		std::cerr << "Error opening file for reading." << std::endl;
	}
	inFile.close();

	return model;
}

// Function to read ASCII STL file similar to your custom style
void readSTL(Polyhedra& stl, const std::string& filepath) {

	stl.clear();

	std::vector<p3>& positions = stl.positions;
	std::vector<p3>& normals = stl.normals;

	/*std::string basePath = "Resources/stl models/";*/
	std::ifstream inFile(/*basePath + */filepath);



	if (!inFile) {
		std::cerr << "Unable to open file: " << /*basePath + */filepath << std::endl;
		return;
	}

	std::string line;
	p3 normal, vertex;

	while (std::getline(inFile, line)) {
		std::istringstream iss(line);
		std::string word;
		iss >> word;

		if (word == "facet") {
			// Read normal vector
			iss >> word; // Skip "normal"
			iss >> normal.x >> normal.y >> normal.z;
			normals.push_back(normal);
			normals.push_back(normal);
			normals.push_back(normal);
			//print(normal);
		}
		else if (word == "vertex") {
			// Read and store vertex position
			iss >> vertex.x >> vertex.y >> vertex.z;
			vector<p3> interm;
			interm.push_back(vertex);
			positions.insert(positions.end(), { interm[0].x,interm[0].y,interm[0].z });
			//print(vertex);
		}
	}

	inFile.close();

	stl.simpleIndices();
}



/// Split a string by a delimiter (e.g. ',') into a list of tokens
std::vector<std::string> split(const std::string& str, char delimiter) {
	std::vector<std::string> tokens;
	std::stringstream ss(str);
	std::string item;
	while (std::getline(ss, item, delimiter)) {
		tokens.push_back(item);
	}
	return tokens;
}

/// Given a LINESTRING-like substring "1.0 2.0, 3.1 4.2, ..."
/// parse out each "lon lat" and convert to p2
std::vector<p2> parseLineStringCoords(const std::string& coordString) {
	std::vector<p2> points;
	// split by comma -> ["1.0 2.0", "3.1 4.2", ...]
	auto commaSeparated = split(coordString, ',');
	for (auto& cs : commaSeparated) {
		std::stringstream ss(cs);
		float lon, lat;
		ss >> lon >> lat;  // read "1.0 2.0" etc.
		points.push_back({ lon, lat });
	}
	return points;
}

void readWKT(/*std::vector<p2>& positions,*/ vector<vector<p2>> & final, const std::string& filepath)
{
	std::ifstream inFile(filepath);
	if (!inFile) {
		std::cerr << "Unable to open file: " << filepath << std::endl;
		return;
	}

	std::string line;
	while (std::getline(inFile, line)) {
		// For simplicity, check if we have LINESTRING or MULTILINESTRING
		// in that whole line (typical for CSV WKT lines).
		// If you have more columns in CSV, you'd parse out just the WKT column.

		if (line.find("LINESTRING") != std::string::npos) {
			// Example:  "...LINESTRING (1.51417 38.6936,1.49 38.70)..."
			// 1) find substring inside parentheses
			size_t start = line.find("(");
			size_t end = line.rfind(")");
			if (start != std::string::npos && end != std::string::npos && end > start) {
				// e.g. "1.51417 38.6936,1.49 38.70"
				std::string coords = line.substr(start + 1, end - (start + 1));

				// 2) parse coords into p2
				auto pts = parseLineStringCoords(coords);
				//positions.insert(positions.end(), pts.begin(), pts.end());
				final.push_back(pts);
			}
		}
		else if (line.find("MULTILINESTRING") != std::string::npos) {
			// Example: "...MULTILINESTRING ((x1 y1, x2 y2),(x3 y3, x4 y4))..."
			// We'll have an *outer* parentheses pair, then multiple sets inside.
			size_t start = line.find("((");
			size_t end = line.rfind("))");
			if (start != std::string::npos && end != std::string::npos && end > start) {
				// Inside is something like: "x1 y1, x2 y2),(x3 y3, x4 y4"
				std::string multi = line.substr(start + 2, end - (start + 2));

				// Each sub-linestring is in parentheses, so let's split by ")(" 
				// Easiest is to replace ")(" with some delimiter, e.g. '|', then split.

				// naive approach: replace ")(" with "|"
				// (In a robust parser, you'd do more careful scanning.)
				// We'll do it quickly:
				for (size_t pos = 0; pos + 1 < multi.size(); ) {
					if (multi[pos] == ')' && multi[pos + 1] == '(') {
						multi[pos] = '|';
						multi[pos + 1] = '|';
					}
					pos++;
				}

				// Now we can split on "||"
				auto subStrings = split(multi, '|');
				for (auto& sub : subStrings) {
					// sub might look like: "x1 y1, x2 y2, x3 y3"
					// trim extra parentheses if any:
					// e.g. if it starts with ')' or '('
					// (in practice, it might be already stripped, but let's be safe).
					// We can parse the substring as a single LINESTRING:
					auto pts = parseLineStringCoords(sub);
					//positions.insert(positions.end(), pts.begin(), pts.end());
					final.push_back(pts);
				}
			}
		}
		// else if there's "POLYGON" or "MULTIPOLYGON", you can handle similarly if needed
	}

	inFile.close();
}




// Function to write a 3D polyhedra without dividing its surfaces in polygons (positions, normals and indices)
void writeSimplePolyhedra(Polyhedra& stl, string modelPath) 
{

	std::vector<p3>& model = stl.positions;
	const std::vector<p3>& normals = stl.normals;
	const std::vector<unsigned int>& indices = stl.indices;


	std::string basePath = "Resources/Simple polyhedra/";
	//std::string modelPath;

	//// List available models in the directory
	//std::cout << "Current models: " << std::endl;
	//for (const auto& entry : fs::directory_iterator(basePath))
	//{
	//	std::cout << "     " << entry.path().filename() << std::endl;
	//}
	/*std::cout << "Enter model name: ";
	std::cin >> modelPath;*/
	std::string path = basePath + modelPath;
	std::cout << "Setting model in: " << path << std::endl;

	std::ofstream outFile(path, std::ios::binary);
	if (outFile)
	{
		//model
		size_t size = model.size();
		outFile.write(reinterpret_cast<const char*>(&size), sizeof(size));
		outFile.write(reinterpret_cast<const char*>(model.data()), size * sizeof(p3));

		//normals
		size = normals.size();
		outFile.write(reinterpret_cast<const char*>(&size), sizeof(size));
		outFile.write(reinterpret_cast<const char*>(normals.data()), size * sizeof(p3));

		//indices
		size = indices.size();
		outFile.write(reinterpret_cast<const char*>(&size), sizeof(size));
		outFile.write(reinterpret_cast<const char*>(indices.data()), size * sizeof(unsigned int));
	}
	else
	{
		std::cerr << "Error opening file for writing." << std::endl;
	}
	outFile.close();
}






//I've made as an intermediate step to modify the csv data of the map, maybe delete it in the future?
//It's in a weird format, in my head it makes much more sense to store all positions sizes at the start and then all the positions
//, but I need to modify a lot of data, so here we are
void writeVectorOfVectors(const vector<vector<p2>>& model) {
	std::string path = "Resources/Borrar/map.txt";

	std::cout << "Setting model in: " << path << std::endl;

	std::ofstream outFile(path, std::ios::binary);
	if (outFile)
	{
		for (size_t i = 0; i < model.size(); i++)
		{
			size_t size = model[i].size();
			outFile.write(reinterpret_cast<const char*>(&size), sizeof(size_t));
			outFile.write(reinterpret_cast<const char*>(model[i].data()), size * sizeof(p2));
		}
	}
	else
	{
		std::cout << "Error opening file for writing." << std::endl;
	}
	outFile.close();
}

void writeVectorOfVectorsAscii(const std::vector<std::vector<p2>>& model)
{
	std::string path = "Resources/Borrar/map.txt";
	std::ofstream outFile(path);
	if (!outFile)
	{
		std::cout << "Error opening file for writing.\n";
		return;
	}


	for (size_t i = 0; i < model.size(); ++i)
	{
		outFile << model[i].size() << " ";

		for (size_t j = 0; j < model[i].size(); ++j)
		{
			outFile << model[i][j].x << " " << model[i][j].y << ", ";
		}

		outFile << "\n";
	}

	outFile.close();
}

//Now only reads the map
void readVectorOfVectorsAscii(std::vector<std::vector<p2>>& model)
{
	std::string basePath = "Resources/";
	std::string addedPath = "MRS/map.txt";
	std::string path = basePath + addedPath;

	std::ifstream inFile(path);
	if (!inFile)
	{
		std::cerr << "Error opening file for reading: " << path << "\n";
		return;
	}

	std::string line;
	while (std::getline(inFile, line))
	{
		if (line.empty())
			continue;

		std::stringstream ss(line);

		size_t sizeOfInner = 0;
		if (!(ss >> sizeOfInner))
		{
			// If we can't read size, format might be wrong or we reached EOF
			continue;
		}

		std::vector<p2> innerVec;
		innerVec.reserve(sizeOfInner);

		for (size_t i = 0; i < sizeOfInner; ++i)
		{
			p2 point;
			if (!(ss >> point.x >> point.y))
			{
				// Couldn’t read two floats—format error or not enough data
				break;
			}

			// Safely consume one character (the comma), ignoring spaces
			// If there is no comma, it might be at the end of the line, etc.
			// We'll do a small check but won't fail completely if it's missing.
			if (ss.peek() == ',' || ss.peek() == ' ')
				ss.ignore(std::numeric_limits<std::streamsize>::max(), ' '); // ignore spaces
			if (ss.peek() == ',')
				ss.ignore(1, ','); // ignore the comma character

			// Store the point
			innerVec.push_back(point);
		}

		// Add this inner vector to the result
		model.push_back(std::move(innerVec));
	}
}