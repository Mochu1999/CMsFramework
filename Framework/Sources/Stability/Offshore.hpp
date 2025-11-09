#pragma once

#include "Common.hpp"
#include "NewWettedSurface.hpp"
#include "Waves.hpp"
#include "Pendulum.hpp"
#include "UIOffshore.hpp"
#include "Buoy.hpp"
#include "Mooring.hpp"

#include "Overlay2D.hpp"
#include "Graphics.hpp"


//This is the general struct which has instances of Buoy, Fourier, Pendulum, Mooring and generator
struct Offshore
{
	Shader& shader3D;
	Shader& shader2D;
	Shader& shaderText;
	Shader& shader2D_Instanced;
	
	Camera& camera;
	GlobalVariables& gv;
	TimeStruct& tm;

	


	Buoy buoy;
	Pendulum pendulum;
	Waves wv;
	
	//setting the anchor radius arbitrarily at 3 times the depth
	p3 p;
	Mooring line1;

	//[0,1,front,2,3,bottom,4,5,back,6,7,top]
	WettedBody wettedBody;

	

	UIOffshore ui;

	float graf1Val = 0;
	Overlay2D overlay;
	Graphic graphic;
	Graphic graphic2;
	ProgressBar pb;
	
	

	Offshore(Shader& shader3D_, Shader& shader2D_, Shader& shaderText_, Shader& shader2D_Instanced_, Camera& camera_, GlobalVariables& gv_, TimeStruct& tm_)
		: shader3D(shader3D_), shader2D(shader2D_), shaderText(shaderText_), shader2D_Instanced(shader2D_Instanced_), camera(camera_), gv(gv_), tm(tm_)
		, wv(tm_), wettedBody(buoy.positions,buoy.body.indices, wv), pendulum(buoy)
		,line1({ -500,-161,0 }, p)
		, ui(shader3D, shader2D, shaderText, gv, tm, camera, buoy, pendulum,wv,wettedBody,line1)
		, overlay(shader2D, camera)
		, graphic(shader2D, shader2D_Instanced, shaderText, camera, tm, "deltaTheta", { 1400,100 }, pendulum.deltaTheta)
		, graphic2(shader2D, shader2D_Instanced, shaderText, camera, tm, "alternator torque", { 1400,400 }, pendulum.genEnergy)
		, pb(shader2D, shader2D_Instanced, shaderText, camera, tm, { 1400 - 50,700 },"Alternator.lambda",buoy.lambda)
	{
		//gv.isRunning = false;
	}

	int buoyMovement = 1;
	void update()
	{
		//buoy.theta += 0.01;
		buoy.updatePositions();
		
		//print(buoy.theta);
		//print(pendulum.tGen);
		pendulum.deltaTheta = buoy.theta - pendulum.theta;

		

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
			////rotation
			//{
			//	// first kick
			//	buoy.omega += 0.5f * dt * buoy.alpha;

			//	// drift
			//	buoy.theta += dt * buoy.omega;

			//	buoy.calculatePendulumAcceleration();

			//	// second kick
			//	buoy.omega += 0.5f * dt * buoy.alpha;
			//}
			////translation
			//{
			//	buoy.vx += 0.5f * dt * buoy.ax;

			//	buoy.x += dt * buoy.vx;

			//	//calculatePendulumAcceleration();

			//	buoy.vx += 0.5f * dt * buoy.ax;
			//	//print(buoy.ax);
			//}
			

			tm.counterUpdateOffshore--;

			pendulum.computePendulumEnergy();
		}

	}


	void draw()
	{
		update();

		ui.draw();

		/*overlay.draw();

		graphic.draw();
		graphic2.draw();

		pb.draw();*/
	}


	

	
};