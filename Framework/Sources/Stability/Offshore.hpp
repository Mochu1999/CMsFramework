#pragma once

#include "Common.hpp"
#include "NewWettedSurface.hpp"
#include "Fourier.hpp"
#include "Pendulum.hpp"
#include "UIOffshore.hpp"
#include "Buoy.hpp"




//This is the general struct which has instances of Buoy, Fourier, Pendulum, Mooring and generator
struct Offshore
{
	//BORRAR
	unsigned int counter = 1;

	Shader& shader3D;
	Shader& shader2D;
	Shader& shaderText;

	Camera& camera;
	GlobalVariables& gv;
	TimeStruct& tm;

	Fourier fourier;
	

	WettedBody wettedBody; //INTENTA NOMBRAR Polyhedra& wettedBody = WettedBody.wettedBody

	Buoy buoy;
	Pendulum pendulum;
	UIOffshore ui;

	

	Lines3D lines;

	

	Offshore(Shader& shader3D_, Shader& shader2D_, Shader& shaderText_, Camera& camera_, GlobalVariables& gv_, TimeStruct& tm_)
		: shader3D(shader3D_), shader2D(shader2D_), shaderText(shaderText_), camera(camera_), gv(gv_), tm(tm_)
		, fourier(tm_), wettedBody(buoy.body, fourier), pendulum(buoy)
		, ui(shader3D, shader2D, shaderText, gv, tm, camera, pendulum)
	{
		{
			

		}

	}


	void update()
	{
		if (counter > 0)
		{
			if (tm.currentTime > 10)
			{
				buoy.ax = 5;
				counter = 0;
			}

		}

		while (tm.counterUpdateOffshore > 0)
		{
			float dt = tm.offshoreUpdateInterval;

			//pendulum
			{
				// first kick
				pendulum.omega += 0.5f * dt * pendulum.alpha;

				// drift
				pendulum.theta += dt * pendulum.omega;

				pendulum.calculatePendulumAcceleration();

				// second kick
				pendulum.omega += 0.5f * dt * pendulum.alpha;
			}
			//rotation
			{
				buoy.vx += 0.5f * dt * buoy.ax;

				buoy.x += dt * buoy.vx;

				if (buoy.ax > 0) buoy.ax -= 0.1;
				//calculatePendulumAcceleration();

				buoy.vx += 0.5f * dt * buoy.ax;
				//print(x);
			}
			//translation

			tm.counterUpdateOffshore--;

			pendulum.computePendulumEnergy();
		}

	}


	void draw()
	{
		update();


		
		
		{

			transparent();
			//opaque();
			shader3D.setUniform("u_Color", 40.0f / 255.0f, 189.9f / 255.0f, 255.0f / 255.0f, 0.3);
			//fourier.updateWavePositions();
			//fourier.draw();

			{
				//wettedBody.calculateWettedBody();
				vector<p3>interm;
				for (auto i : wettedBody.intersections)
				{
					interm.insert(interm.end(), i.begin(), i.end());
				}
				//lines.clear();
				//lines.addSet(interm);
			}

			opaque();
			glLineWidth(4); //this is deprecated and platform dependent
			shader3D.setUniform("u_fragmentMode", 1);


			shader3D.setUniform("u_Color", 1.0, 0.0, 0.0, 1.0);
			//lines.draw();
		}

		ui.draw();

	}


	

	
};