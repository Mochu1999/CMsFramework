#pragma once

#include "Common.hpp"
#include "NewWettedSurface.hpp"
#include "Fourier.hpp"

struct Offshore
{
	Shader& shader3D;
	Shader& shader2D;
	Shader& shaderText;

	Camera& camera;
	GlobalVariables& gv;
	TimeStruct& tm;

	Fourier fourier;

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

	//Separar el reinicio de lectura del modelo de rhino por otro lado?
	Polyhedra stl,cube;

	p3 lightPos = { 10,50,10 };

	Offshore(Shader& shader3D_, Shader& shader2D_, Shader& shaderText_, Camera& camera_, GlobalVariables& gv_, TimeStruct& tm_)
		: shader3D(shader3D_), shader2D(shader2D_), shaderText(shaderText_), camera(camera_), gv(gv_), tm(tm_)
		,fourier(tm_)
	{
		{
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
			print(wavePeriod);
			k = waveFrequency * waveFrequency / g;
			print(waveFrequency);


			Ffk = C33 * waveAmplitude * exp(-k * draft);
			print(Ffk);

			RAO = Ffk / waveAmplitude / (-waveFrequency * waveFrequency * (m + A33) + C33);
			print(RAO);

			RAO = Ffk / waveAmplitude / pow(pow((-waveFrequency * waveFrequency * (m + A33) + C33), 2) + pow(waveFrequency * damping, 2), 0.5);
			print(RAO);


			//printOffshore();
		}
		
		//readSTL(stl, "Cube.stl");
		//writeSimplePolyhedra(stl, "Cube.bin");
		cube.addPolyhedra("Cube.bin");
	}

	void draw()
	{
		shader3D.bind();
		shader3D.setUniform("u_Model", gv.identityMatrix);
		shader3D.setUniform("u_fragmentMode", 0);
		opaque();
		shader3D.setUniform("u_Color", 1, 1, 1, 1);
		cube.draw();
		transparent();
		//opaque();
		shader3D.setUniform("u_Color", 40.0f / 255.0f, 189.9f / 255.0f, 255.0f / 255.0f, 0.5);
		fourier.updateWavePositions();
		fourier.draw();
		

		

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