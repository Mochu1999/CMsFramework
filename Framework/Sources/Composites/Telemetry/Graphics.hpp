#pragma once
#include "Common.hpp"
#include "Circles.hpp"
#include "Lines2D_Instanced.hpp"
#include "Light.hpp"
#include "Text.hpp"



struct Graphic {

	Shader& shader2D;
	Shader& shader2D_Instanced;
	Shader& shaderText;

	TimeStruct& tm;

	Camera& camera;
	Light& ship;

	Lines2D_Instanced gridStatic;
	Lines2D_Instanced gridDynamic, currentXgrid;

	Lines2D frame;
	Lines2D data;

	Text text;
	vector<Line> lines;

	string name;

	std::array<float, 16> graphicModel2DMatrix;


	unsigned int counterSecondsPlot = 0; //will be compared to the variables of the same name in tm to know when to update
	unsigned int counterUpdatePlot = 0;

	//OUTERCORNER ES DEL GRID, NO DEL FRAME
	float counter = 0;
	p2 gridCorner, dataCorner;
	float minDataX = 0, maxDataX = 0, minDataY = 0, maxDataY = 0;
	float scaleY = 4;
	float gridWidth = 400, gridHeight = 200;
	float currentX = 0;
	float& currentY;

	Graphic(Shader& shader2D_, Shader& shader2D_Instanced_, Shader& shaderText_, Camera& camera_, Light& ship_, TimeStruct& tm_
		, string name_, p2 gridCorner_,float& currentY_)
		: shader2D(shader2D_), shader2D_Instanced(shader2D_Instanced_), shaderText(shaderText_), tm(tm_)
		, camera(camera_), ship(ship_), text("resources/Glyphs/Helvetica/Helvetica.otf", 16),
		name(name_), gridCorner(gridCorner_), currentY(currentY_)
	{
		gridStatic.addSet({ {0,0},{1,0} });
		gridDynamic.addSet({ {0,0},{1,0} });
		currentXgrid.addSet({ {0,0},{1,0} });

		gridStatic.addInstances({ //why do I have the scale as a feature if I am using the model matrix for it
			//horizontal
			{ {0, 0},  0, {1, 1} },
			//vertical
			{ {0, 0},  radians(90), {1, 1} },
			});

		frame.addSet(createRoundedSquare({ gridCorner.x - 50,gridCorner.y - 50 }, { 500,300 }, 25));



		text.fillVertexBuffer();

		update();
	}

	deque<float> verticalAuxGridValues = { 0 };
	vector<Line> verticalAuxGridText; //LINE ES UN NOMBRE DE MIERDA PORQUE SE CONFUNDE CON LINES_2D
	vector<InstanceAttributes> auxGridLines;

	void updateAuxGrid() {
		if (tm.counterSecondsPlot != counterSecondsPlot)
		{
			counterSecondsPlot = tm.counterSecondsPlot;
			verticalAuxGridValues.push_back(tm.currentTime);

		}
	}

	void updateText() {

		//VARIABLES LOCALES PARA COSAS COMUNES

		float interm1 = gridCorner.x - 40;


		if ((maxDataY - minDataY) * scaleY < 20)
			lines = {
					{{interm1,gridCorner.y + (maxDataY - minDataY) * scaleY + 10}, round1d(maxDataY)},
					{{interm1,gridCorner.y }, round1d(minDataY)} };

		else if ((maxDataY - minDataY) * scaleY < 30)
			lines = { {{interm1,gridCorner.y + (maxDataY - minDataY) * scaleY}, round1d(maxDataY)},
					{{interm1,gridCorner.y }, round1d(minDataY)} };

		else
			lines = {
					{{interm1,gridCorner.y - minDataY * scaleY}, 0},
					{{interm1,gridCorner.y + (maxDataY - minDataY) * scaleY}, round1d(maxDataY)},
					{{interm1,gridCorner.y }, round1d(minDataY)} };

		vector<Line> currentData = {
			{ {gridCorner.x + currentX - maxDataX,gridCorner.y - 20},round1d(tm.currentTime)," s"},
			{ {gridCorner.x + currentX - maxDataX + 10,gridCorner.y + (data.positions.back().y - minDataY) * scaleY},round1d(currentY)}
		};



		verticalAuxGridText.clear();
		for (size_t i = 0; i < verticalAuxGridValues.size(); i++)
		{
			//LO MISMO ESTO EN TEXT ESTÁ MAL SI updateDynamicGrid LO USA
			if (tm.currentTime * 50 - verticalAuxGridValues.front() * 50 >= gridWidth - 10)
			{
				verticalAuxGridValues.pop_front();
			}

			verticalAuxGridText.push_back(
				{ {gridCorner.x - maxDataX + verticalAuxGridValues[i] * 50 - 10,gridCorner.y - 20},round1d(verticalAuxGridValues[i]) }
			);
		}



		vector<Line> staticLegend = {
			{ {gridCorner.x + 20,gridCorner.y + gridHeight + 10}," ",name} };

		lines.insert(lines.end(), currentData.begin(), currentData.end());
		lines.insert(lines.end(), verticalAuxGridText.begin(), verticalAuxGridText.end());
		lines.insert(lines.end(), staticLegend.begin(), staticLegend.end());
		text.addDynamicText(lines);
	}

	void updateDynamicGrid() {

		auxGridLines.clear();

		//0 horizontal line
		auxGridLines.push_back({ {gridCorner.x,gridCorner.y - minDataY * scaleY},  0, {gridWidth, 1} });

		auxGridLines.push_back({ {gridCorner.x,gridCorner.y + (maxDataY - minDataY) * scaleY},  0, {gridWidth, 1} });

		for (size_t i = 0; i < verticalAuxGridValues.size(); i++)
		{
			auxGridLines.push_back(
				{ { gridCorner.x - maxDataX + verticalAuxGridValues[i] * 50,gridCorner.y},radians(90) ,{gridHeight,1} });

			auxGridLines.push_back({ { 100, 0 }, radians(90), { 1, 1 } });
		}


		gridDynamic.addInstances(auxGridLines);
	}

	void update() {
		
		pushData();

		dataCorner = gridCorner - p2{ maxDataX,minDataY * scaleY };

		updateAuxGrid();
		updateText();
		updateDynamicGrid();

	}

	

	

	void pushData() {
		

		currentX = tm.currentTime * 50;

		if (currentY < minDataY) minDataY = currentY;
		if (currentY > maxDataY) maxDataY = currentY;
		if (currentX > gridWidth) maxDataX = currentX - 400;
		if ((maxDataY - minDataY)*scaleY > gridHeight) scaleY = gridHeight / (maxDataY - minDataY);

		data.positions.push_back({ currentX,currentY });

		createIndices();
		data.isBufferUpdated = true;
	}
	void createIndices() {
		size_t size = data.positions.size();
		if (size > 1)
		{
			data.indices.push_back(size - 2);
			data.indices.push_back(size - 1);
		}
	}

	void draw() {
		if (tm.counterUpdatePlot != counterUpdatePlot)
		{
			counterUpdatePlot++;
			update();
		}


		//LOS MODEL NO NECESITAN SER ACTUALIZADOS CADA FRAME,HACER CONDICIONAL


		//Grid
		{
			shader2D_Instanced.bind();

			graphicModel2DMatrix = camera.create2DModelMatrix(gridCorner, 0, { gridWidth,gridHeight });
			shader2D_Instanced.setUniform("u_Model", graphicModel2DMatrix);

			shader2D.setUniform("u_Color", 1, 1, 1, 1);
			gridStatic.draw();

			shader2D_Instanced.setUniform("u_Model", camera.identityMatrix);

			shader2D_Instanced.setUniform("u_Color", 0.5, 0.5, 0.5, 0.5);
			gridDynamic.draw();

			shader2D_Instanced.setUniform("u_Color", 1, 0, 0, 1);

			currentXgrid.addInstances({
				{ { gridCorner.x - maxDataX + currentX,gridCorner.y},radians(90) ,{(data.positions.back().y - minDataY) * scaleY,1} },
				{{gridCorner.x,gridCorner.y + (data.positions.back().y - minDataY) * scaleY},0,{currentX - maxDataX,1}}
				});
			currentXgrid.draw();
		}


		//roundedSquare
		{
			transparent();
			shader2D.bind();
			shader2D.setUniform("u_Color", 40.0f / 255.0f, 239.9f / 255.0f, 239.0f / 255.0f, 1);
			glLineWidth(3);
			shader2D_Instanced.setUniform("u_Model", camera.identityMatrix);
			frame.draw();
			glLineWidth(1);
		}


		//Data
		{
			shader2D_Instanced.setUniform("u_Color", 40.0f / 255.0f, 239.9f / 255.0f, 239.0f / 255.0f, 1);
			graphicModel2DMatrix = camera.create2DModelMatrix(dataCorner, 0, { 1,scaleY });
			shader2D_Instanced.setUniform("u_Model", graphicModel2DMatrix);
			glEnable(GL_SCISSOR_TEST); //You'll need to try deque method just for potencial memory leaks
			glScissor(gridCorner.x, 0, windowWidth, windowHeight);
			data.draw();
			glDisable(GL_SCISSOR_TEST);
		}





		shaderText.bind();

		text.draw();

	}

};





