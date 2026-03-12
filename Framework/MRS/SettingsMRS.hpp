#pragma once


#include "Common.hpp"
#include "Camera.hpp"

struct SettingsMRS
{
	std::string settingsPath = "Resources/Settings/SettingsMRS.bin";

	p3& cameraPos;
	p3& forward;
	CameraModes& cameraMode;

	enum Variables
	{
		CameraPos,
		Forward,
		CameraMode
	};

	SettingsMRS(Camera& camera, GlobalVariables& gv)
		: cameraPos(camera.cameraPos), forward(camera.forward), cameraMode(gv.cameraMode)
	{
		read();
		//reset();
		//write();
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

			var = CameraMode;
			outFile.write(reinterpret_cast<const char*>(&var), sizeof(var));
			outFile.write(reinterpret_cast<const char*>(&cameraMode), sizeof(cameraMode));

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
				case CameraMode:
					inFile.read(reinterpret_cast<char*>(&cameraMode), sizeof(cameraMode));
					if (cameraMode == FPS)cameraMode = drag; //FPS mouse bugs if it's set at the start
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
	
	void reset()
	{
		cameraPos = { -10,0,0 };
		forward = { 1,0,0 };
		cameraMode = centered;
	}
};