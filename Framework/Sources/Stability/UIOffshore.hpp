#pragma once

struct UIOffshore
{
	Shader& shader3D;
	Shader& shader2D;
	Shader& shaderText;
	GlobalVariables& gv;
	TimeStruct& tm;
	Camera& camera;

	Pendulum& pendulum;

	//aux
	Text text;
	Text textAux;
	Lines2D centerCross;
	Polygons2D dataBox;
	Lines2D dataBoxOutline;
	Text dataText;

	Axis axis;

	p3 lightPos = { 10,50,10 };

	matrix4x4 model3DMatrix = camera.identityMatrix;

	UIOffshore(Shader& shader3D_, Shader& shader2D_, Shader& shaderText_, GlobalVariables& gv_, TimeStruct& tm_
		, Camera& camera_, Pendulum& pendulum_)
		:shader3D(shader3D_), shader2D(shader2D_), shaderText(shaderText_), gv(gv_), tm(tm_),camera(camera_)
		, pendulum(pendulum_)
		, text("resources/Glyphs/Helvetica/Helvetica.otf", 36), textAux("resources/Glyphs/Helvetica/Helvetica.otf", 48)
		, dataText("resources/Glyphs/Helvetica/Helvetica.otf", 20)
		, axis(shader3D, gv)
	{
		activateLight();

		//Aux
		{
			text.addText({ {{ 10,950 }, tm.fps, " fps"},{{10,1000},tm.currentTime, " s"} });


			centerCross.addSet({
				{gv.centerWindow.x - 20,gv.centerWindow.y},{gv.centerWindow.x + 20,gv.centerWindow.y},
				{gv.centerWindow.x,gv.centerWindow.y - 20},{gv.centerWindow.x,gv.centerWindow.y + 20} });
			centerCross.indices = { 0,1,2,3 };

			vector<p2> dataRectangle = createRoundedSquare(windowTotal - p2{ 300,100 }, windowTotal, 30);
			dataBoxOutline.addSet(dataRectangle);
			dataBox.addSet(dataRectangle);
		}
	}


	void draw()
	{
		shader3D.bind();
		shader3D.setUniform("u_Model", gv.identityMatrix);
		shader3D.setUniform("u_fragmentMode", 0);
		opaque();

		shader3D.setUniform("u_Color", 1, 1, 1, 1);
		//body.draw();

		
		drawPendulum();


		drawAux();

		axis.draw(); //breaking the "opaque first" rule to get the axis' color unaffected by water
	}
	void drawPendulum()
	{

		camera.rotate3DModelMatrix(model3DMatrix, degrees(pendulum.theta), { 0,0,1 });
		//camera.translate3DModelMatrix(model3DMatrix, { buoy.x,0,0 });
		shader3D.setUniform("u_Model", model3DMatrix);

		shader3D.setUniform("u_Color", 1, 1, 1, 1);

		pendulum.body.draw();
	}


	void drawAux()
	{

		transparent();
		shaderText.bind();
		text.draw();
		text.substituteText(0, { { 10,950 }, round2d(tm.fps), " fps" });
		text.substituteText(1, round1d(tm.currentTime), " s");


		transparent();
		shader2D.bind();
		shader2D.setUniform("u_Model", gv.identityMatrix);
		shader2D.setUniform("u_Color", 1, 1, 1, 0.5);
		glLineWidth(2);
		centerCross.draw();
		glLineWidth(1);
		opaque();



		transparent();
		shader2D.bind();
		shader2D.setUniform("u_Model", gv.identityMatrix);
		shader2D.setUniform("u_Color", 40 / 255.0f, 40 / 255.0f, 40 / 255.0f, 1.0f);
		dataBox.draw();
		shader2D.setUniform("u_Color", 40.0f / 255.0f, 239.9f / 255.0f, 239.0f / 255.0f, 1);
		glLineWidth(3);
		dataBoxOutline.draw();
		glLineWidth(1);

		shaderText.bind();
		dataText.addDynamicText({
			{windowTotal - p2{280,60}, "Total Energy: ",pendulum.totalEnergy}
			});
		dataText.draw();
	}

	void activateLight()
	{
		shader3D.bind();
		shader3D.setUniform("u_lightPos", lightPos);
	}
};
