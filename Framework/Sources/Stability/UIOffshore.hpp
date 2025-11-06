#pragma once

struct UIOffshore
{
	Shader& shader3D;
	Shader& shader2D;
	Shader& shaderText;
	GlobalVariables& gv;
	TimeStruct& tm;
	Camera& camera;

	Buoy& buoy;
	Pendulum& pendulum;

	Waves& wv;
	WettedBody& wettedBody;
	Lines3D lines;

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
		, Camera& camera_, Buoy& buoy_, Pendulum& pendulum_, Waves& wv_, WettedBody& wettedBody_)
		:shader3D(shader3D_), shader2D(shader2D_), shaderText(shaderText_), gv(gv_), tm(tm_), camera(camera_)
		, buoy(buoy_), pendulum(pendulum_), wv(wv_), wettedBody(wettedBody_)
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

		//buoy.body.draw();
		shader3D.setUniform("u_Color", 0.1, 0.1, 1, 1);

		camera.rotate3DModelMatrix(model3DMatrix, degrees(buoy.theta), { 0,0,1 });
		camera.translate3DModelMatrix(model3DMatrix, { buoy.x,0,0 });
		shader3D.setUniform("u_Model", model3DMatrix);
		buoy.support.draw();


		shader3D.setUniform("u_Color", 1, 0, 0, 1);
		buoy.alternator.draw();

		drawPendulum();

		//resetting identity matrix for wettedBoddy
		shader3D.setUniform("u_Model", gv.identityMatrix);



		{

			transparent();
			//opaque();
			shader3D.setUniform("u_Color", 40.0f / 255.0f, 189.9f / 255.0f, 255.0f / 255.0f, 0.3);
			wv.updateWavePositions();
			//wv.draw();
			shader3D.setUniform("u_Color", 1, 1, 1,0.2);
			camera.rotate3DModelMatrix(model3DMatrix, degrees(buoy.theta), { 0,0,1 });
			camera.translate3DModelMatrix(model3DMatrix, { buoy.x,0,0 });
			shader3D.setUniform("u_Model", model3DMatrix);
			buoy.body.draw();


			{
				vector<p3>interm;
				for (auto i : wettedBody.intersections)
				{
					interm.insert(interm.end(), i.begin(), i.end());
				}
				lines.clear();
				lines.addSet(interm);

			}

			opaque();
			glLineWidth(4); //this is deprecated and platform dependent
			shader3D.setUniform("u_fragmentMode", 1);


			shader3D.setUniform("u_Color", 1.0, 0.0, 0.0, 1.0);
			//lines.draw();


			wettedBody.calculateWettedBody();
			shader3D.setUniform("u_fragmentMode", 0);

			//wettedBody.wet.draw();

			shader3D.setUniform("u_fragmentMode", 0);

		}

		axis.draw(); //breaking the "opaque first" rule to get the axis' color unaffected by water

		drawAux();
	}




	void drawPendulum()
	{

		camera.rotate3DModelMatrix(model3DMatrix, degrees(pendulum.theta), { 0,0,1 });
		camera.translate3DModelMatrix(model3DMatrix, { buoy.x,0,0 });

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



		/*transparent();
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
		dataText.draw();*/
	}

	void activateLight()
	{
		shader3D.bind();
		shader3D.setUniform("u_lightPos", lightPos);
	}
};
