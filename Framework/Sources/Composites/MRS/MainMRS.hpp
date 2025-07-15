#pragma once

//pocketGrib

#include "FilesManagement.hpp"
#include "Circles.hpp"
#include "Text.hpp"
#include "Pathfinding.hpp"
#include "ShipMRS.hpp"
#include "Meteo.hpp"



//CAMERA ESTÁ SOLO PARA LAS MODEL MATRICES, PLANTEATE SI TE INTERESA TENER una referencia de camera, porque bien podría solo tener
// una matriz aquí local y hacer la función que la crea global
	//Crear matrices locales permitirá no recrear la matriz cada vez a coste de mayor memoria y no necesitar camara en muchas structs


struct ShipMRS
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




string formatFloat(float value)
{
	std::ostringstream oss;
	oss << std::fixed << std::setprecision(2) << value;
	return oss.str();
}


string lonLatToString(p2 lonLat)
{
	string lonStr, latStr;
	float lon = lonLat.x, lat = lonLat.y;

	if (lon >= 0)
		lonStr = formatFloat(lon) + "E";
	else if (lon < 0)
		lonStr = formatFloat(-lon) + "W";

	if (lat >= 0)
		latStr = formatFloat(lat) + "N";
	else if (lat < 0)
		latStr = formatFloat(-lat) + "S";

	return "{" + lonStr + ", " + latStr + "}";
}



//Ver que está en uso, que no, reserves y comentarios
struct MainMap
{
	TimeStruct& tm;
	Shader& shader2D, shaderText, shader2D_Instanced;
	Camera& camera;
	GlobalVariables& gv;

	Lines2D mercator;
	Polygons2D background;

	shipMRSData shipMRS;
	Meteo meteo;

	struct MapData
	{
		vector<p2> lonLatFrame;
		vector<p2> mercatorFrame;

		vector<p2> lonLatPositions;
		vector<p2> mercatorPositions;


		MapData()
		{

		}
	};


	

	


	matrix4x4 mapModel2DMatrix;

	p2 point0;
	float totalX, totalY, naturalRatio, scalingFactor;
	float totalPixels = 1000 * 6; //This makes totalX 6000 pixels long //Stored in settings

	p2 mapCorner, translationTotal;
	vector<float> frameLimits; //[0] x left, [1] x right, [2] y bottom, [3] y up
	bool show = 1;

	Text text1, text2;
	
	Polygons2D dataBox;

	Polygons2D mouseBox;

	Lines2D frame;
	Lines2D dataBoxOutline;

	MainMap(Shader& shader2D_, Shader& shaderText_, Shader& shader2D_Instanced_, Camera& camera_, GlobalVariables& gv_, TimeStruct& tm_)
		:shader2D(shader2D_), shaderText(shaderText_), shader2D_Instanced(shader2D_Instanced_), tm(tm_),
		camera(camera_), text1("resources/Glyphs/Helvetica/Helvetica.otf", 36), text2("resources/Glyphs/Helvetica/Helvetica.otf", 36)
		, gv(gv_), meteo(shader2D, shader2D_Instanced, camera, tm), shipMRS(shader2D,camera,tm)
	{

		vector<vector<p2>> mapVectorOfVectors;
		readVectorOfVectorsAscii(mapVectorOfVectors);

		//Starting with the first point after the frame
		for (size_t i = 1; i < mapVectorOfVectors.size(); i++)
		{
			vector<p2>interm = lonLatToMercator(mapVectorOfVectors[i]);
			mercator.addSet(interm);
		}

		frame.addSet(lonLatToMercator(mapVectorOfVectors[0]));

		background.addSet(frame.positions);

		//bottom left corner in meters (mercator) from earth's 0,0 
		point0 = frame.positions[0];

		totalX = frame.positions[1].x - frame.positions[0].x;
		totalY = frame.positions[2].y - frame.positions[1].y;
		//print(totalX); //4.22809e+06
		//print(totalY); //3.52124e+06
		//print(totalX / totalY); //1.20074



		

		

		dataBoxOutline.addSet(createRoundedSquare({ 80,370 }, { 600,380 }, 30));
		dataBox.addSet(createRoundedSquare({ 80,370 }, { 600,380 }, 30));


		update();


		


	}

	void update()
	{


		//Multiplying the map positions by scalingFactor in the model matrix will make all coordinates fall between 0 and totalPixels (6000 pixels)
		scalingFactor = 1 / totalX * totalPixels;

		//after scalingFactor, point0 falls in {-500.242,652.636}. We will sum that amount to put point0 in O
		translationTotal = { -point0.x * scalingFactor, -point0.y * scalingFactor };
		//we end with a centered left corner and a map that is totalPixels long


		//after having the map in the corner we move it to an arbitrary half of the scali
		mapCorner = { windowWidth / 2 - totalPixels / 2,windowHeight / 2 - totalPixels / 2 };
		translationTotal += mapCorner;

		/*print(translationTotal);
		print(scalingFactor);*/


		frameLimits.insert(frameLimits.end(), { frame.positions[0].x, frame.positions[1].x, frame.positions[0].y , frame.positions[2].y });
		//se usa frame para algo? .clear?



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
			shipMRS.speed =   meteo.getCurrentSpeed()/3;
			shipMRS.updateShipPosition();
			//gv.isRunning = 0;
		}

		
	}

	void draw()
	{
		updateTime();

		p2 a = (lonLatToMercator(shipMRS.shipCoordinates) - point0) * scalingFactor + mapCorner;
		p2 cursorVal = mercatorToLonLat((gv.mPos - translationTotal) / scalingFactor);



		//shipCoordinates = cursorVal; //en movimiento el barco rota con un delay de un segundo, por qué?

		shader2D.bind();
		transparent();


		mapModel2DMatrix = camera.create2DModelMatrix(translationTotal, 0, scalingFactor);
		shader2D.setUniform("u_Model", mapModel2DMatrix);

		shader2D.setUniform("u_Color", 0.035f, 0.065f, 0.085f, 1.0f);
		background.draw();


		shader2D.setUniform("u_Color", 40.0f / 255.0f, 239.9f / 255.0f, 239.0f / 255.0f, 1);
		mercator.draw();
		shader2D.setUniform("u_Color", 1, 1, 1, 1);

		glLineWidth(3);
		frame.draw();
		glLineWidth(1);


		//print(0.65346 * earthRadius /1000);

		meteo.draw(mapModel2DMatrix);

		shipMRS.mapModel2DMatrix = mapModel2DMatrix;
		shipMRS.draw(a);





		if (show)
		{
			mapModel2DMatrix = camera.create2DModelMatrix(gv.mPos, 0, 1);
			shader2D.setUniform("u_Model", camera.identityMatrix);

			shader2D.setUniform("u_Color", 40 / 255.0f, 40 / 255.0f, 40 / 255.0f, 1.0f);
			mouseBox.clear();
			mouseBox.addSet(createRoundedSquare({ gv.mPos.x - 4,gv.mPos.y - 10 }, { 270,42 }, 10));
			//mouseBox.draw(); //Tío, esto tendría que hacerse con model, no cada vez

			shaderText.bind();
			text2.addDynamicText({ {gv.mPos, lonLatToString(cursorVal)} });
			//text2.draw();



			shader2D.bind();
			shader2D.setUniform("u_Model", camera.identityMatrix);
			shader2D.setUniform("u_Color", 40 / 255.0f, 40 / 255.0f, 40 / 255.0f, 1.0f);
			dataBox.draw();
			shader2D.setUniform("u_Color", 40.0f / 255.0f, 239.9f / 255.0f, 239.0f / 255.0f, 1);
			glLineWidth(3);
			dataBoxOutline.draw();
			glLineWidth(1);

			shaderText.bind();
			float eta = round1d(shipMRS.totalDistance / shipMRS.speed / 3600);
			if (shipMRS.speed == 0) eta = 0;
			text1.addDynamicText({
				{{ 100,700 }, "Ship coordinates:  ", lonLatToString(shipMRS.shipCoordinates)},
				{ { 100,650 }, "Distance left:  ", round1d(shipMRS.totalDistance / 1000 )," km"},
				{ { 100,600 }, "Speed: ", round1d(meterSecondToKnot(shipMRS.speed)) ,"  knots"},
				{ { 100,550 }, "Estimated time left: ",eta," hours"},
				{ { 100,500 }, "Errors:  N/A"},
				{{620,900},meteo.currentTimeString}
				});

			text1.draw();
		}
		//shader2D.setUniform("u_Model", camera.identityMatrix);





	}
};
