#pragma once

#include "Common.hpp"
#include "NewWettedSurface.hpp"
#include "Fourier.hpp"
#include "Pendulum.hpp"

//This is the struct of the Buoy which has instances of Fourier, Pendulum, Mooring and generator

struct Offshore
{
	unsigned int counter = 1;

	Shader& shader3D;
	Shader& shader2D;
	Shader& shaderText;

	Camera& camera;
	GlobalVariables& gv;
	TimeStruct& tm;

	Fourier fourier;
	//Separar el reinicio de lectura del modelo de rhino por otro lado //una struct que gestione si pasar polyhedra o leer stl
	Polyhedra body;

	WettedBody wettedBody; //INTENTA NOMBRAR Polyhedra& wettedBody = WettedBody.wettedBody

	Pendulum pendulum;

	Polygons2D dataBox;
	Lines2D dataBoxOutline;
	Text dataText;

	float rho = 1025, g = 9.81;
	float draft = 80;
	float m;

	float A33; //added mass
	float C33; //restoring coefficient

	float naturalFrequency, waveFrequency; //rad/s
	float period, wavePeriod; //s
	float k;//1/m

	float waveHeight = 2, waveAmplitude = waveHeight / 2; //m

	float criticalDamping, damping;
	float Ffk;
	float RAO;

	float x = 0;
	float y = 0;
	float vx = 0;
	float vy = 0;
	float ax = 5;
	float ay = 0;

	float theta = PI * 2 / 3;// rad
	float omega = 0;// rad/s
	float alpha = 0;// rad/s^2

	Lines3D lines;

	p3 lightPos = { 10,50,10 };

	Offshore(Shader& shader3D_, Shader& shader2D_, Shader& shaderText_, Camera& camera_, GlobalVariables& gv_, TimeStruct& tm_)
		: shader3D(shader3D_), shader2D(shader2D_), shaderText(shaderText_), camera(camera_), gv(gv_), tm(tm_)
		, fourier(tm_), wettedBody(body, fourier), pendulum(shader3D_, camera, tm_, x, omega, ax, ay)
		, dataText("resources/Glyphs/Helvetica/Helvetica.otf", 20)
	{
		{
			vector<p2> dataRectangle = createRoundedSquare(windowTotal - p2{ 300,100 }, windowTotal, 30);
			dataBoxOutline.addSet(dataRectangle);
			dataBox.addSet(dataRectangle);

			activateLight();

			float D = 20, T = 80;
			p3 cog = { 0,0,0 };

			waveFrequency = 1.5;

			float area = PI * D * D / 4;

			m = area * T * rho;

			C33 = rho * g * area;

			A33 = (2.0 / 3 * PI * D * D * D / 8) * rho; //Volume of the semi sphere

			naturalFrequency = pow(C33 / (m + A33), 0.5);

			period = 2 * PI / naturalFrequency;

			criticalDamping = 2 * pow((m + A33) * C33, 0.5);
			damping = criticalDamping * 0.05;

			wavePeriod = 2.0 * PI / waveFrequency;
			//print(wavePeriod);
			k = waveFrequency * waveFrequency / g;
			//print(waveFrequency);


			Ffk = C33 * waveAmplitude * exp(-k * draft);
			//print(Ffk);

			RAO = Ffk / waveAmplitude / (-waveFrequency * waveFrequency * (m + A33) + C33);
			//print(RAO);

			RAO = Ffk / waveAmplitude / pow(pow((-waveFrequency * waveFrequency * (m + A33) + C33), 2) + pow(waveFrequency * damping, 2), 0.5);
			//print(RAO);


			//printOffshore();

			print(pendulum.naturalPeriod);
		}

		//Polyhedra stl;
		//readSTL(stl, "body.stl");
		//writeSimplePolyhedra(stl, "body.bin");
		body.addPolyhedra("Cube.bin");



	}


	void update()
	{
		if (counter > 0)
		{
			if (tm.currentTime > 10)
			{
				ax = 5;
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
				vx += 0.5f * dt * ax;

				x += dt * vx;

				if (ax > 0) ax -= 0.1;
				//calculatePendulumAcceleration();

				vx += 0.5f * dt * ax;
				print(x);
			}
			//translation

			tm.counterUpdateOffshore--;

			pendulum.computePendulumEnergy();
		}

	}


	void draw()
	{
		update();

		shader3D.bind();
		shader3D.setUniform("u_Model", gv.identityMatrix);
		shader3D.setUniform("u_fragmentMode", 0);
		opaque();
		shader3D.setUniform("u_Color", 1, 1, 1, 1);
		//body.draw();
		pendulum.draw();

		//PENDULUM
		{
			shader3D.setUniform("u_Color", 1, 1, 1, 1);
			pendulum.draw();
		}


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

		drawData();
	}

	void drawData()
	{
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

	void printOffshore()
	{
		print(A33);
		print(C33);
		print(naturalFrequency);
		print(period);
	}

	void activateLight()
	{
		shader3D.bind();
		shader3D.setUniform("u_lightPos", lightPos);
	}
};