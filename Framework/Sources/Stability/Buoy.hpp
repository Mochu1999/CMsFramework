#pragma once

struct Buoy
{
	Polyhedra body;
	Polyhedra support;
	Polyhedra alternator;

	float x = 0;
	float y = 0;
	float vx = 0;
	float vy = 0;
	float ax = 0;
	float ay = 0;

	float theta = radians(30);// rad
	float omega = 0;// rad/s
	float alpha = 0;// rad/s^2

	float lambda=1;

	//EXCEL, A CAMBIAR
	float draft = 80;
	float m;

	float A33; //added mass
	float C33; //restoring coefficient

	float naturalFrequency, waveFrequency; //rad/s
	float period, wavePeriod; //s
	float k;//1/m

	float criticalDamping, damping;
	float Ffk;
	float RAO;

	float waveHeight = 2, waveAmplitude = waveHeight / 2; //m

	Buoy()
	{
		/*Polyhedra stl;
		readSTL(stl, "Alternator.stl");
		writeSimplePolyhedra(stl, "Alternator.bin");

		readSTL(stl, "Buoy.stl");
		writeSimplePolyhedra(stl, "Buoy.bin");

		readSTL(stl, "Support.stl");
		writeSimplePolyhedra(stl, "Support.bin");*/

		body.addPolyhedra("Cube.bin");
		alternator.addPolyhedra("Alternator.bin");
		support.addPolyhedra("Support.bin");

		excel();

	}
	void calculatePendulumAcceleration()
	{
		//Just for testing purposes
		float tWeight = -(g / 10) * sinf(theta);

		alpha = tWeight;

	}
	void excel()
	{
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
	}
	void printOffshore()
	{
		print(A33);
		print(C33);
		print(naturalFrequency);
		print(period);
	}
};