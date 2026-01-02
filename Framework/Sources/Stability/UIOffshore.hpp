#pragma once
#include "Mooring.hpp"
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
	Mooring& line1;
	Mooring& line2;
	Mooring& line3;
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
	Arrows2 arrows;

	matrix4x4 model3DMatrix = identityMatrix;

	UIOffshore(Shader& shader3D_, Shader& shader2D_, Shader& shaderText_, GlobalVariables& gv_, TimeStruct& tm_
		, Camera& camera_, Buoy& buoy_, Pendulum& pendulum_, Waves& wv_, WettedBody& wettedBody_
		, Mooring& line1_, Mooring& line2_, Mooring& line3_, Arrows2& arrows_)

		:shader3D(shader3D_), shader2D(shader2D_), shaderText(shaderText_), gv(gv_), tm(tm_), camera(camera_)
		, buoy(buoy_), pendulum(pendulum_), wv(wv_), wettedBody(wettedBody_)
		, line1(line1_), line2(line2_), line3(line3_)
		, text("resources/Glyphs/Helvetica/Helvetica.otf", 36), textAux("resources/Glyphs/Helvetica/Helvetica.otf", 48)
		, dataText("resources/Glyphs/Helvetica/Helvetica.otf", 20)
		, axis(shader3D, gv)
		, arrows(arrows_)
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
		shader3D.setUniform("u_Model", identityMatrix);
		shader3D.setUniform("u_fragmentMode", 0);
		opaque();


		//buoy.body.draw();
		//shader3D.setUniform("u_Color", 0.1, 0.1, 1, 1);

		




		{
			vector<p3>interm;
			for (auto i : wettedBody.intersections)
			{
				interm.insert(interm.end(), i.begin(), i.end());
			}
			lines.clear();
			lines.addSet(interm);

		}

		


		

		{
			opaque();
			//opaque();
			shader3D.setUniform("u_Color", 40.0f / 255.0f, 189.9f / 255.0f, 255.0f / 255.0f, 0.3);
			wv.updateWavePositions();
			wv.draw();
		}

		{//BUOY
			opaque();
			//opaque();
			shader3D.setUniform("u_Color", 1, 1, 1, 1);
			buoy.body.draw();
		}

		{//wet
			opaque();
			glLineWidth(4); //this is deprecated and platform dependent
			shader3D.setUniform("u_fragmentMode", 1);


			shader3D.setUniform("u_Color", 1.0, 0.0, 0.0, 1.0);
			lines.draw();
			wettedBody.calculateWettedBody();
			shader3D.setUniform("u_fragmentMode", 1);
			wettedBody.wet.draw();
			shader3D.setUniform("u_fragmentMode", 0);
		}
		drawMooring();

		{

			camera.rotate3DModelMatrix(model3DMatrix, degrees(buoy.theta), { 0,0,1 });
			camera.translate3DModelMatrix(model3DMatrix, { buoy.x,buoy.y,0 });
			shader3D.setUniform("u_Model", model3DMatrix);
			shader3D.setUniform("u_Color", 1, 0.5, 0, 1);
			buoy.support.draw();
			//shader3D.setUniform("u_Color", 1, 0, 0, 1);
			//buoy.alternator.draw();
			drawPendulum();

			shader3D.setUniform("u_Model", identityMatrix);
		}


		{
			vector<p3> ps;
			vector<p3> ns;
			vector<float> fs;

			for (auto& i : wettedBody.faces)
			{
				ps.push_back(i.centroid);
				ns.push_back(i.n);
				//fs.push_back(log(i.force));
				fs.push_back(i.force/1000000);
			}
			/*print(ps);
			print(ns);*/
			arrows.addSet(ps, ns,fs);
			//arrows.draw(); //not working, don't know why, so taking them to axis.extra
			axis.extra.clear();
			axis.extra.addSet(arrows.positions,2);
			axis.draw(); //breaking the "opaque first" rule to get the axis' color unaffected by water
		}

		//drawAux();
	}


	void drawMooring()
	{
		shader3D.bind();
		shader3D.setUniform("u_Model", identityMatrix);

		glLineWidth(3);
		shader3D.setUniform("u_fragmentMode", 1);


		shader3D.setUniform("u_Color", 1, 0.7, 0.5, 1);
		line1.line.draw();
		line2.line.draw();
		line3.line.draw();

		glLineWidth(1);

		shader3D.setUniform("u_Color", 1, 1, 1, 1.0);
		shader3D.setUniform("u_fragmentMode", 0);

	}

	void drawPendulum()
	{

		camera.rotate3DModelMatrix(model3DMatrix, degrees(pendulum.theta), { 0,0,1 });
		camera.translate3DModelMatrix(model3DMatrix, { buoy.x,buoy.y,0 });

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
		shader2D.setUniform("u_Model", identityMatrix);
		shader2D.setUniform("u_Color", 1, 1, 1, 0.5);
		glLineWidth(2);
		//centerCross.draw();
		glLineWidth(1);
		opaque();



		/*transparent();
		shader2D.bind();
		shader2D.setUniform("u_Model", identityMatrix);
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
