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
#include "Arrows.hpp"

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
	Mooring line2;
	Mooring line3;

	//[0,1,front,2,3,bottom,4,5,back,6,7,top]
	WettedBody wettedBody;



	UIOffshore ui;

	float graf1Val = 0;
	Overlay2D overlay;
	Graphic graphic;
	Graphic graphic2;
	ProgressBar pb;

	Arrows2 arrows;


	vector<p3>wetForces;
	vector<p3>wetN;

	Offshore(Shader& shader3D_, Shader& shader2D_, Shader& shaderText_, Shader& shader2D_Instanced_, Camera& camera_, GlobalVariables& gv_, TimeStruct& tm_)
		: shader3D(shader3D_), shader2D(shader2D_), shaderText(shaderText_), shader2D_Instanced(shader2D_Instanced_), camera(camera_), gv(gv_), tm(tm_)
		, wv(tm_), wettedBody(buoy.positions, buoy.body.indices, wv), pendulum(buoy)
		, line1({ -300,-161,0 }, buoy.connectionPoint), line2({ 150,-161,-259 }, buoy.connectionPoint), line3({ 150,-161,259 }, buoy.connectionPoint)
		, ui(shader3D, shader2D, shaderText, gv, tm, camera, buoy, pendulum, wv, wettedBody, line1,line2,line3, arrows)
		, overlay(shader2D, camera)
		, graphic(shader2D, shader2D_Instanced, shaderText, camera, tm, "deltaTheta [rad]", { 1400,100 }, pendulum.deltaTheta)
		, graphic2(shader2D, shader2D_Instanced, shaderText, camera, tm, "alternator torque [N·m]", { 1400,400 }, pendulum.genEnergy)
		, pb(shader2D, shader2D_Instanced, shaderText, camera, tm, { 1400 - 50,700 }, "Alternator.lambda", buoy.lambda)
	{
		//gv.isRunning = false;
		//print("a");
	}

	int buoyMovement = 1;
	void update()
	{
		line1.update();
		line2.update();
		line3.update();

		//retrieving wet faces data

		wetForces.clear();
		wetN.clear();
		for (auto& i : wettedBody.faces)
		{
			wetForces.push_back(i.forceVec);
			wetN.push_back(i.n);
		}
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
			//Pitch
			{
				// first kick
				buoy.omega += 0.5f * dt * buoy.alpha;

				// drift
				buoy.theta += dt * buoy.omega;

				calculatePitchAcceleration();

				// second kick
				buoy.omega += 0.5f * dt * buoy.alpha;
			}
			//Surge
			{
				buoy.vx += 0.5f * dt * buoy.ax;

				buoy.x += dt * buoy.vx;

				calculateSurgeAcceleration();

				buoy.vx += 0.5f * dt * buoy.ax;
				//print(buoy.ax);
			}
			//Heave
			{
				buoy.vy += 0.5f * dt * buoy.ay;

				buoy.y += dt * buoy.vy;

				calculateHeaveAcceleration();

				buoy.vy += 0.5f * dt * buoy.ay;
			}
			

			tm.counterUpdateOffshore--;

			pendulum.computePendulumEnergy();
		}

	}
	
	void calculatePitchAcceleration()
	{
		float Mz = 0.0;
		p3 CG = { buoy.x, buoy.y-1.5f, 0.0f };

		for (size_t i = 0; i < wetForces.size(); ++i)
		{
			p3 r = wettedBody.faces[i].centroid - CG;
			p3 F = wetForces[i];

			Mz += r.x * F.y - r.y * F.x;
		}

		//catenary horizontal forces
		const p3 Tsum = line1.tension + line2.tension + line3.tension;
		const p3 Th = { Tsum.x, 0.0f, 0.0f }; 
		const p3 rc = buoy.connectionPoint - CG;
		Mz +=  - rc.y * Th.x;

		float bPitch = 1e3;     // [N·dm·s/rad] damping
		float Md = -bPitch * buoy.omega;

		float Iz = 54754157/3; //kg*m^2
		buoy.alpha = (Mz /*+ Md*/) / Iz;   // rotational acceleration
	}
	void calculateHeaveAcceleration()
	{
		float Fy = 0;

		for (unsigned int i = 0; i < wetForces.size(); ++i)
		{

			Fy += wetForces[i].y;
		}
		float Ffloater = 34775.8f * 3.0f;
		Fy += Ffloater;

		float Fcatenary = line1.tension.y + line2.tension.y + line3.tension.y;
		Fy += Fcatenary;

		const float b = 5.7e4f * 10; // [N·s/m]
		float Fd = -b * buoy.vy; // damping

		buoy.ay = (Fy+Fd - buoy.m * g) / buoy.m;
		//print(buoy.ay);

	}
	void calculateSurgeAcceleration()
	{
		float Fx = 0;

		for (unsigned int i = 0; i < wetForces.size(); ++i)
		{

			Fx += wetForces[i].x;
		}

		float Fcatenary = line1.tension.x + line2.tension.x + line3.tension.x;
		Fx += Fcatenary*pow(buoy.x,2);
		//print(Fcatenary);

		const float b = 7.142e4f*4; // [N·s/m]
		float Fd = -b * buoy.vy; // damping

		buoy.ax = (Fx+Fd) / buoy.m/2;
		//print(buoy.ax);

	}

	void draw()
	{
		update();

		ui.draw();

		overlay.draw();

		graphic.draw();
		graphic2.draw();

		pb.draw();
	}





};