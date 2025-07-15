#pragma once

#include <filesystem>
namespace fs = std::filesystem;



struct BinariesManager {

	struct IndexEntry {
		enum DataType { ProgramType, ModelPath };
		DataType type;
		size_t offset;
		size_t size;
	};
	/*for (const auto& entry : indexEntries) {
	std::cout << "Type: ";
	if (entry.type == IndexEntry::ProgramType) {
		std::cout << "ProgramType, ";
	}
	else if (entry.type == IndexEntry::ModelPath) {
		std::cout << "ModelPath, ";
	}
	std::cout << "Offset: " << entry.offset << ", Size: " << entry.size << std::endl;
	}
	cout << "MODELPATH " << modelPath << endl;*/

	std::string basePath = "Resources/";

	enum programType { CAD, RUNNING };

	//these two are read from config
	//programType currentProgramType = RUNNING;
	//std::string modelPath = "1-goliath.bin";
	programType currentProgramType;
	std::string modelPath;

	IndexEntry indexEntries[2];
	BinariesManager() {

		//writeConfig();
		readConfig();
	}


	//si cierras writeConfig sin terminar el proceso config desaparece, mira a ver
	void writeConfig() {
		system("cls");

		std::string input;

		bool foundModelPath = false;




		cout << "set currentProgramType { CAD, RUNNING }: ";
		std::cin >> input;
		if (input == "CAD" || input == "0")
		{
			currentProgramType = CAD;
		}
		else if (input == "RUNNING" || input == "1")
		{
			currentProgramType = RUNNING;
		}
		else
		{
			cout << "error invalid programType name" << endl;
			return;
		}



		if (currentProgramType == RUNNING)
		{
			std::string possibleModelPath;
			cout << "set model path to one of these: " << endl;
			for (const auto& entry : fs::directory_iterator(basePath)) {
				std::cout << "     " << entry.path().filename() << std::endl;
			}
			std::cin >> possibleModelPath;


			for (const auto& entry : fs::directory_iterator(basePath)) {
				if (possibleModelPath == entry.path().filename())
				{
					foundModelPath = true;
					break;
				}
			}
			if (foundModelPath)
			{

				modelPath = possibleModelPath;

				std::ofstream outFile("Resources/config.bin", std::ios::binary);
				if (outFile)
				{

					//now that we have the values of programType and modelPath, we set indexEntries and write it all
					indexEntries[0] = { IndexEntry::ProgramType, sizeof(indexEntries), sizeof(programType) };
					indexEntries[1] = { IndexEntry::ModelPath, sizeof(indexEntries) + sizeof(programType), modelPath.size() };

					for (const auto& entry : indexEntries)
					{
						outFile.write(reinterpret_cast<const char*>(&entry), sizeof(entry));
					}
					outFile.write(reinterpret_cast<const char*>(&currentProgramType), sizeof(currentProgramType));
					outFile.write(modelPath.data(), modelPath.size());

				}
				outFile.close();
			}
			else
				cout << "file not found" << endl;

		}
		else if (currentProgramType == CAD)
		{
			cout << "CAD" << endl;

			std::ofstream outFile("Resources/config.bin", std::ios::binary);
			if (outFile)
			{

				//now that we have the values of programType and modelPath, we set indexEntries and write it all
				indexEntries[0] = { IndexEntry::ProgramType, sizeof(indexEntries), sizeof(programType) };
				indexEntries[1] = { IndexEntry::ModelPath, sizeof(indexEntries) + sizeof(programType), modelPath.size() };

				for (const auto& entry : indexEntries)
				{
					outFile.write(reinterpret_cast<const char*>(&entry), sizeof(entry));
				}
				outFile.write(reinterpret_cast<const char*>(&currentProgramType), sizeof(currentProgramType));
			}
			outFile.close();

		}


	}

	void readConfig() {
		//system("cls");


		std::ifstream inFileConfig("Resources/config.bin", std::ios::binary);

		if (inFileConfig)
		{
			//sets all indexEntries
			inFileConfig.read(reinterpret_cast<char*>(indexEntries), sizeof(indexEntries));


			inFileConfig.seekg(indexEntries[0].offset);
			inFileConfig.read(reinterpret_cast<char*>(&currentProgramType), indexEntries[0].size);



			inFileConfig.seekg(indexEntries[1].offset);
			modelPath.resize(indexEntries[1].size);

			if (inFileConfig.read(&modelPath[0], modelPath.size())) {
				std::cout << endl << "modelPath: " << modelPath << std::endl << std::endl;
			}

		}
		else
		{
			std::cout << "config not found? This should not happen" << endl;
		}
		inFileConfig.close();

	}

	void writeModel(std::vector<float> model) {
		cout << "save you model, currently if the file the are these ones:" << endl;
		for (const auto& entry : fs::directory_iterator(basePath)) {
			std::cout << "     " << entry.path().filename() << std::endl;
		}
		std::cin >> modelPath;
		std::string path = basePath + modelPath;
		cout << "setting model in: " << path << endl;

		std::ofstream outFile(path, std::ios::binary);
		if (outFile)
		{
			size_t size = model.size();
			outFile.write(reinterpret_cast<const char*>(&size), sizeof(size));
			outFile.write(reinterpret_cast<const char*>(model.data()), size * sizeof(int));
		}
		outFile.close();

	}

	std::vector<float> readModel()
	{
		std::vector<float> model;
		std::string path = basePath + modelPath;

		std::ifstream inFile(path, std::ios::binary);

		if (inFile)
		{
			size_t size;
			inFile.read(reinterpret_cast<char*>(&size), sizeof(size));
			model.resize(size);
			inFile.read(reinterpret_cast<char*>(model.data()), size * sizeof(int));

		}
		inFile.close();

		return model;
	}
};