#pragma once

#include <filesystem>
namespace fs = std::filesystem;

#include "Common.hpp"
#include "Camera.hpp"


/*struct Settings
{
	std::string settingsPath = "Resources/Settings.bin";

	p3& cameraPos;
	p3& forward;
	GlobalVariables& gv;
	Mainworld& world;



	enum Variables
	{
		CameraPos,
		Forward,
		Program,
		TotalPixels,
		ModelMatrixOCC,
		TotalMiddleMPosVariation
	};

	struct VariableEntry //a location to each var
	{
		Variables id; //we can't just have a vector<void*> or the setting would need to be perfectly ordered each time
		void* ptr;
		size_t size; //you can't make sizeof(*entry.ptr) as it is of type void. So this is the only way to store the size
	};

	std::vector<VariableEntry> entries =
	{
		{CameraPos, &cameraPos, sizeof(cameraPos)},
		{Forward, &forward, sizeof(forward)},
		{Program, &gv.program, sizeof(gv.program)},
		{TotalPixels, &world.totalPixels, sizeof(world.totalPixels)},
		{ModelMatrixOCC, &gv.modelMatrixOCC, sizeof(gv.modelMatrixOCC)},
		{TotalMiddleMPosVariation, &gv.totalMiddleMPosVariation, sizeof(gv.totalMiddleMPosVariation)}
	};

	std::unordered_world<Variables, VariableEntry*> entryworld;

	Settings(Camera& camera, GlobalVariables& gv_, Mainworld& world_) : cameraPos(camera.cameraPos), forward(camera.forward), gv(gv_), world(world_)
	{
		for (auto& entry : entries)
			entryworld[entry.id] = &entry;

		read();
	}

	//It first writes the enum value of the variable, and then the variable
	void write()
	{
		std::ofstream outFile(settingsPath, std::ios::binary);
		if (outFile)
		{
			for (const auto& entry : entries)
			{
				outFile.write(reinterpret_cast<const char*>(&entry.id), sizeof(entry.id));
				outFile.write(reinterpret_cast<const char*>(entry.ptr), sizeof(entry.size));
			}
		}
		outFile.close();
	}
//NECESITA UNA VUELTA
	////Each variable is stored in two parts. First the enum variable is read, and then it will enter a switch case to read its associate
	//void read()
	//{
	//	std::ifstream inFile(settingsPath, std::ios::binary);

	//	if (inFile)
	//	{
	//		while (inFile)
	//		{
	//			Variables var;
	//			if (!inFile.read(reinterpret_cast<char*>(&var), sizeof(var))) break;

	//			auto it = entryworld.find(var);
	//			if (it != entryworld.end())
	//				inFile.read(reinterpret_cast<char*>(it->second->ptr), it->second->size);
	//			else
	//			{
	//				std::cerr << "Unknown variable in settings file." << std::endl;
	//				break;
	//			}
	//		}
	//	}
	//	else
	//	{
	//		std::cout << "Settings not found? This should not happen" << std::endl; //maybe create them empty if so...
	//	}
	//	inFile.close();

	//}
};
*/


//Old settings

struct Settings
{
	std::string settingsPath = "Resources/Settings.bin";

	p3& cameraPos;
	p3& forward;
	GlobalVariables& gv;
	World& world;

	enum Variables
	{
		CameraPos,
		Forward,
		Program,
		TotalPixels,
		ModelMatrixOCC,
		TotalMiddleMPosVariation
	};

	Settings(Camera& camera, GlobalVariables& gv_, World& world_) : cameraPos(camera.cameraPos), forward(camera.forward), gv(gv_), world(world_)
	{
		read();
		reset();
		//printVars();
	}

	//It first writes the enum value of the variable, and then the variable
	void write()
	{
		std::ofstream outFile(settingsPath, std::ios::binary);
		if (outFile)
		{
			Variables var = CameraPos;
			outFile.write(reinterpret_cast<const char*>(&var), sizeof(var));
			outFile.write(reinterpret_cast<const char*>(&cameraPos), sizeof(cameraPos));

			var = Forward;
			outFile.write(reinterpret_cast<const char*>(&var), sizeof(var));
			outFile.write(reinterpret_cast<const char*>(&forward), sizeof(forward));

			var = Program;
			outFile.write(reinterpret_cast<const char*>(&var), sizeof(var));
			outFile.write(reinterpret_cast<const char*>(&gv.program), sizeof(gv.program));

			var = TotalPixels;
			outFile.write(reinterpret_cast<const char*>(&var), sizeof(var));
			outFile.write(reinterpret_cast<const char*>(&world.totalPixels), sizeof(world.totalPixels));

			var = ModelMatrixOCC;
			outFile.write(reinterpret_cast<const char*>(&var), sizeof(var));
			outFile.write(reinterpret_cast<const char*>(&gv.modelMatrixOCC), sizeof(gv.modelMatrixOCC));

			var = TotalMiddleMPosVariation;
			outFile.write(reinterpret_cast<const char*>(&var), sizeof(var));
			outFile.write(reinterpret_cast<const char*>(&gv.totalMiddleMPosVariation), sizeof(gv.totalMiddleMPosVariation));
		}
		outFile.close();
	}

	//Each variable is stored in two parts. First the enum variable is read, and then it will enter a switch case to read its associate
	void read()
	{
		std::ifstream inFile(settingsPath, std::ios::binary);

		if (inFile)
		{
			while (inFile)
			{
				Variables var;
				if (!inFile.read(reinterpret_cast<char*>(&var), sizeof(var))) break;

				switch (var)
				{
				case CameraPos:
					inFile.read(reinterpret_cast<char*>(&cameraPos), sizeof(cameraPos));
					break;
				case Forward:
					inFile.read(reinterpret_cast<char*>(&forward), sizeof(forward));
					break;
				case Program:
					inFile.read(reinterpret_cast<char*>(&gv.program), sizeof(gv.program));
					break;
				case TotalPixels:
					inFile.read(reinterpret_cast<char*>(&world.totalPixels), sizeof(world.totalPixels));
					world.update(); //if we end reducir all thise, just world.update() at the end of read()
					break;
				case ModelMatrixOCC:
					inFile.read(reinterpret_cast<char*>(&gv.modelMatrixOCC), sizeof(gv.modelMatrixOCC));
					break;
				case TotalMiddleMPosVariation:
					inFile.read(reinterpret_cast<char*>(&gv.totalMiddleMPosVariation), sizeof(gv.totalMiddleMPosVariation));
					break;


				default:
					std::cerr << "Unknown variable in settings file." << std::endl;
					break;
				}
			}
		}
		else
		{
			std::cout << "Settings not found? This should not happen" << std::endl;
		}
		inFile.close();


	}
	void printVars()
	{
		print(cameraPos);
		print(forward);
		print(world.totalPixels);
		print(gv.program);
		print(gv.modelMatrixOCC);
		print(gv.totalMiddleMPosVariation);
	}
	void reset()
	{
		/*cameraPos = { 0,0,0 };
		forward = { 1,0,0 };
		world.totalPixels = 6000;*/
		gv.modelMatrixOCC = gv.identityMatrix;
		gv.totalMiddleMPosVariation = { 0,0,0 };
	}
};
