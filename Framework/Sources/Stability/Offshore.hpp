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
	Shader& shader3D;
	Shader& shader2D;
	Shader& shaderText;

	Camera& camera;
	GlobalVariables& gv;
	TimeStruct& tm;

	

	WettedBody wettedBody; //INTENTA NOMBRAR Polyhedra& wettedBody = WettedBody.wettedBody
	Fourier fourier;

	Buoy buoy;
	Pendulum pendulum;
	UIOffshore ui;

	

	

	

	Offshore(Shader& shader3D_, Shader& shader2D_, Shader& shaderText_, Camera& camera_, GlobalVariables& gv_, TimeStruct& tm_)
		: shader3D(shader3D_), shader2D(shader2D_), shaderText(shaderText_), camera(camera_), gv(gv_), tm(tm_)
		, fourier(tm_), wettedBody(buoy.body, fourier), pendulum(buoy)
		, ui(shader3D, shader2D, shaderText, gv, tm, camera, buoy, pendulum,fourier,wettedBody)
	{

	}


	void update()
	{

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
			//translation
			{
				buoy.vx += 0.5f * dt * buoy.ax;

				buoy.x += dt * buoy.vx;

				//calculatePendulumAcceleration();

				buoy.vx += 0.5f * dt * buoy.ax;
				print(buoy.ax);
			}
			

			tm.counterUpdateOffshore--;

			pendulum.computePendulumEnergy();
		}

	}


	void draw()
	{
		update();

		ui.draw();
		
	}


	

	
};