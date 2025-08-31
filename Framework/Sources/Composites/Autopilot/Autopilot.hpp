#pragma once

//pocketGrib

#include "FilesManagement.hpp"
#include "Circles.hpp"
#include "Text.hpp"
#include "Pathfinding.hpp"
#include "ShipAutopilot.hpp"
#include "Meteo.hpp"
#include "AuxAutopilot.hpp"
#include "World.hpp"
#include "UIAutopilot.hpp"

struct Autopilot
{

	TimeStruct& tm;
	Shader& shader2D, shaderText, shader2D_Instanced;
	GlobalVariables& gv;

	World world;
	ShipAutopilot ship;
	Meteo meteo;
	UIAutopilot ui;



	Autopilot(Shader& shader2D_, Shader& shaderText_, Shader& shader2D_Instanced_, GlobalVariables& gv_, TimeStruct& tm_)
		:shader2D(shader2D_), shaderText(shaderText_), shader2D_Instanced(shader2D_Instanced_), tm(tm_)
		, gv(gv_)
		, meteo(tm)
		, ship(tm, world.scalingFactor, world.translationFactor)
		, ui(shader2D, shader2D_Instanced, shaderText, gv, world, ship, meteo)
	{
		


	}

	void updateTime() {

		if (tm.counterUpdateMRS > 0)
		{
			tm.counterUpdateMRS--;


			int day = meteo.currentTimeKey / 10000;
			int hour = (meteo.currentTimeKey / 100) % 100;
			int minute = meteo.currentTimeKey % 100;

			// Advance 1 hour
			hour++;
			if (hour >= 24)
			{
				hour = 0;
				day++;
			}

			meteo.currentTimeKey = day * 10000 + hour * 100 + minute;
			//print(currentTimeKey);

			//displayed time string
			meteo.currentTimeString = "2024-May-" + std::to_string(day) + ", " + std::to_string(hour) + ":" + std::to_string(minute);


			meteo.extractData(meteo.currentTimeKey);
			ship.speed = meteo.getCurrentSpeed() / 3;
			ship.updateShipPosition();
		}
	}

	void update()
	{
		updateTime();


		ui.draw();

	}
};


//Sin Implementar
struct Algo
{
	//////////inputs
	p2 lonLatPosition;
	float globalCourse;
	float driftAngle;
	//Custom realTimeRepeatedMeteorology

	float windSpeed;
	p2 windDirection;

	struct AIS
	{
		p2 position;
		float speed;
		float course;

	};
	vector<AIS> aisShips;
	float batteryLevel;


	//////////outputs
	string telemEtryMRS;
	unsigned int pulsesRudder;
	unsigned int pulsesFlap1;
	unsigned int pulsesFlap2;
	unsigned int pulsesFlap3; //direction

};