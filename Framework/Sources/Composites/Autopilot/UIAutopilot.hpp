#pragma once
#include "Shader.hpp"
#include "Text.hpp"
#include "Polygons2D.hpp"
#include "Lines2D.hpp"
#include "World.hpp"
#include "Icons.hpp"

//Having all the UI in this higher level clears the classes, removes shader and mapModel2DMatrix references
// and facilitate changing this matrix. But stuff is messy over here and I still find weird not to have them in their respective class


struct UIAutopilot
{
	Shader& shader2D;
	Shader& shader2D_Instanced;
	Shader& shaderText;

	GlobalVariables& gv;

	World& world;
	ShipAutopilot& ship;
	Meteo& meteo;

	IconLight icon;
	matrix4x4 mapModel2DMatrix;

	bool show = 0;
	Text mouseCoordsText;
	Polygons2D mouseCoordsBox;

	Polygons2D dataBox;
	Lines2D dataBoxOutline;
	Text dataText;
	Text meteoDateText;

	Circles circleFinish;



	UIAutopilot(Shader& shader2D_, Shader& shader2D_Instanced_, Shader& shaderText_, GlobalVariables& gv_, World& world_, ShipAutopilot& ship_, Meteo& meteo_)
		:shader2D(shader2D_), shader2D_Instanced(shader2D_Instanced_), shaderText(shaderText_), gv(gv_), world(world_), ship(ship_), meteo(meteo_)
		, mouseCoordsText("resources/Glyphs/Helvetica/Helvetica.otf", 36), dataText("resources/Glyphs/Helvetica/Helvetica.otf", 36)
		, meteoDateText("resources/Glyphs/Helvetica/Helvetica.otf", 36)
		, circleFinish(5000, 100)
	{
		dataBoxOutline.addSet(createRoundedSquare({ 80,370 }, { 600,380 }, 30));
		dataBox.addSet(createRoundedSquare({ 80,370 }, { 600,380 }, 30));



	}

	void draw()
	{
		//put here all model matrix changes
		shader2D.bind();
		transparent();

		mapModel2DMatrix = create2DModelMatrix(world.translationFactor, 0, world.scalingFactor);
		shader2D.setUniform("u_Model", mapModel2DMatrix);

		drawWorld();

		drawMeteo();
		drawShip();

		drawData();

		drawMouseCoords();
	}

	void drawWorld()
	{
		shader2D.setUniform("u_Color", 0.035f, 0.065f, 0.085f, 1.0f);
		world.background.draw();

		shader2D.setUniform("u_Color", 40 / 255.0f, 40 / 255.0f, 40 / 255.0f, 1.0f);
		shader2D.setUniform("u_Color", 1, 1, 1, 1);
		for (auto& algo : world.polygons)
		{
			//algo.draw();
		}

		shader2D.setUniform("u_Color", 40.0f / 255.0f, 239.9f / 255.0f, 239.0f / 255.0f, 1);
		world.mercator.draw();

		shader2D.setUniform("u_Color", 1,0,0,1);
		world.mercatorSafe.draw();
		

		glLineWidth(3);
		world.frame.draw();
		glLineWidth(1);
	}

	void drawShip()
	{
		shader2D.bind();

		circleFinish.clear();
		circleFinish.addSet(lonLatToMercator(ship.pathWaypoints.front()));
		//circleFinish.addSet(lonLatToMercator(ship.pathWaypoints));
		circleFinish.addSet(lonLatToMercator(ship.pathWaypoints.back()));


		shader2D.setUniform("u_Color", 1, 0, 0, 1);
		circleFinish.drawInterior();

		//The icon requires a different model matrix because it is naturally drawn at the origin instead of at it mercator points








		//The icon is located at 0,0 without model matrix
		p2 shipMercator = lonLatToMercator(ship.shipCoordinates);
		//We will move it into it shipMercator coordinates, to later multiply it scalingFactor and sum translationFactor
		matrix4x4 shipLocal = create2DModelMatrix(shipMercator, ship.angle, 50.0f / world.scalingFactor);


		//I know this doesn't seem intuitive, but the final matrix will have an scale of 50, and a translation equal
		// to shipMercator*scalingFactor+translationFactor
		matrix4x4 shipModel = multiplyMatrices(mapModel2DMatrix, shipLocal);


		shader2D.setUniform("u_Model", shipModel);


		drawIcon();
		shader2D.setUniform("u_Model", mapModel2DMatrix);



		shader2D.setUniform("u_Color", 1, 0.3, 0, 0.5);
		glLineWidth(1);
		ship.courseLine.draw();
		glLineWidth(1);
	}

	void drawIcon()
	{
		shader2D.setUniform("u_Color", 1, 1, 0, 1);
		icon.mainHull.draw();
		icon.sideHull1.draw();
		icon.sideHull2.draw();

		shader2D.setUniform("u_Color", 1, 1, 1, 1);
		icon.aux1.draw();
		icon.aux2.draw();
		icon.aux3.draw();

		/*shader2D.setUniform("u_Color", 1, 0, 0, 1);
		glLineWidth(3);
		auxa.draw();
		auxb.draw();
		glLineWidth(1);*/
	}

	void drawMeteo()
	{
		shader2D_Instanced.bind();


		shader2D_Instanced.setUniform("u_Model", mapModel2DMatrix);
		shader2D_Instanced.setUniform("u_Color", 1, 1, 1, 1);
		meteo.arrows.lines.draw();

		shaderText.bind();
		meteoDateText.addDynamicText({ {{620,900},meteo.currentTimeString} });
		meteoDateText.draw();
		
	}

	void drawData()
	{
		shader2D.bind();
		shader2D.setUniform("u_Model", gv.identityMatrix);
		shader2D.setUniform("u_Color", 40 / 255.0f, 40 / 255.0f, 40 / 255.0f, 1.0f);
		dataBox.draw();
		shader2D.setUniform("u_Color", 40.0f / 255.0f, 239.9f / 255.0f, 239.0f / 255.0f, 1);
		glLineWidth(3);
		dataBoxOutline.draw();
		glLineWidth(1);

		shaderText.bind();
		float eta = round1d(ship.totalDistance / ship.speed / 3600);
		if (ship.speed == 0) eta = 0;
		dataText.addDynamicText({
			{{ 100,700 }, "Ship coordinates:  ", lonLatToString(ship.shipCoordinates)},
			{ { 100,650 }, "Distance left:  ", round1d(ship.totalDistance / 1000)," km"},
			{ { 100,600 }, "Speed: ", round1d(meterSecondToKnot(ship.speed)) ,"  knots"},
			{ { 100,550 }, "Estimated time left: ",eta," hours"},
			{ { 100,500 }, "Errors:  N/A"}
			});
		dataText.draw();
		
		
	}

	void drawMouseCoords()
	{
		p2 cursorVal = mercatorToLonLat((gv.mPos - world.translationFactor) / world.scalingFactor);

		if (show)
		{
			shader2D.bind();
			mapModel2DMatrix = create2DModelMatrix(gv.mPos, 0, 1);
			shader2D.setUniform("u_Model", gv.identityMatrix);

			shader2D.setUniform("u_Color", 40 / 255.0f, 40 / 255.0f, 40 / 255.0f, 1.0f);
			mouseCoordsBox.clear();
			//mouseCoordsBox should be static and moved with the modelMatrix, now it is getting recreated each frame
			mouseCoordsBox.addSet(createRoundedSquare({ gv.mPos.x - 4,gv.mPos.y - 10 }, { 270,42 }, 10));
			mouseCoordsBox.draw();

			shaderText.bind();
			mouseCoordsText.addDynamicText({ {gv.mPos, lonLatToString(cursorVal)} });
			mouseCoordsText.draw();
		}
	}
};