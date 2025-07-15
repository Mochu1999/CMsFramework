
#pragma once
#include "Text.hpp"

struct ProgressBar {

	Shader& shader2D;
	Shader& shader2D_Instanced;
	Shader& shaderText;

	TimeStruct& tm;

	Camera& camera;
	Light& ship;

	unsigned int counterUpdatePlot = 0;

	Text text;

	Lines2D outerArc;
	Polygons2D innerArc;
	p2 outerCorner;
	p2 outerLength = { 500,50 };
	p2 innerLength;
	float naturalRatio = outerLength.y / outerLength.x;

	float percentage = 1, variation = 0.0005;

	ProgressBar(Shader& shader2D_, Shader& shader2D_Instanced_, Shader& shaderText_, Camera& camera_, Light& ship_, TimeStruct& tm_
		, p2 outerCorner_)
		: shader2D(shader2D_), shader2D_Instanced(shader2D_Instanced_), shaderText(shaderText_), tm(tm_)
		, camera(camera_), ship(ship_), text("resources/Glyphs/Helvetica/Helvetica.otf", 20), outerCorner(outerCorner_)
	{
		float algo1 = 0.03;
		float algo2 = 0.1 * naturalRatio;
		innerLength = { outerLength.x * algo2, outerLength.y * algo1 };
		outerArc.addSet(createRoundedSquare(outerCorner, outerLength, 25));
		//innerArc.addSet(createRoundedSquare(outerCorner + innerLength, outerLength - innerLength * 2, 25 * (1 - algo1)));
		innerArc.addSet(createRoundedSquare(outerCorner, outerLength, 25));
	}

	void update(){
		if (percentage >= 1 || percentage <= 0)
			variation = -variation;

		percentage += variation;

		text.addDynamicText(
			{ { {outerCorner.x + 30,outerCorner.y + outerLength.y + 10} ,"Battery"},
			{ {outerCorner.x + outerLength.x * percentage,outerCorner.y - 25} ,round1d(percentage * 100)},
			{ {outerCorner.x + outerLength.x * percentage + 40,outerCorner.y - 25} ," %"} });
	}

	void draw() {

		if (tm.counterUpdatePlot != counterUpdatePlot)
		{
			counterUpdatePlot++;
			update();
		}

		
		transparent();
		shader2D.bind();
		shader2D_Instanced.setUniform("u_Model", camera.identityMatrix);

		shader2D.setUniform("u_Color", 1, 1, 1, 1);
		shader2D.setUniform("u_Color", 0.7, 0.7, 0.7, 1);
		glEnable(GL_SCISSOR_TEST); //You'll need to try deque method just for potencial memory leaks

		glScissor(outerCorner.x - 10, 0, 10 + outerLength.x * percentage, windowHeight);
		innerArc.draw();
		glDisable(GL_SCISSOR_TEST);

		//shader2D.setUniform("u_Color", 1, 0, 0, 1);
		shader2D.setUniform("u_Color", 40.0f / 255.0f, 239.9f / 255.0f, 239.0f / 255.0f, 1);
		glLineWidth(3);

		outerArc.draw();
		glLineWidth(1);

		shaderText.bind();



		

		text.draw();

	}
};