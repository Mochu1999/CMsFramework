#pragma once

#include "Common.hpp"

struct Offshore
{
	double rho = 1025, g = 9.81;
	double draft = 80;
	double m;

	double A33; //added mass
	double C33; //restoring coefficient

	double naturalFrequency, waveFrequency; //rad/s
	double period, wavePeriod; //s
	double k;//1/m

	double waveHeight = 2, waveAmplitude = waveHeight / 2; //m

	double criticalDamping, damping;
	double Ffk;
	double RAO;

	Offshore()
	{
		double D = 20, T = 80;

		waveFrequency = 1.5;

		double area = PI * D * D / 4;

		m = area * T * rho;

		C33 = rho * g * area;

		A33 = (2.0 / 3 * PI * D * D * D / 8) * rho; //Volume of the semi sphere

		naturalFrequency = pow(C33 / (m + A33), 0.5);

		period = 2 * PI / naturalFrequency;

		criticalDamping = 2 * pow((m + A33) * C33, 0.5);
		damping = criticalDamping * 0.05;

		wavePeriod = 2.0 * PI / waveFrequency;
		print(waveFrequency);
		k = waveFrequency * waveFrequency / g;
		print(waveFrequency);


		Ffk = C33 * waveAmplitude * exp(-k * draft);
		print(Ffk);

		RAO = Ffk / waveAmplitude / (-waveFrequency * waveFrequency * (m + A33) + C33);
		print(RAO);

		RAO = Ffk / waveAmplitude / pow(pow((-waveFrequency * waveFrequency * (m + A33) + C33),2)+pow(waveFrequency*damping,2),0.5);
		print(RAO);


		//printOffshore();
	}

	void printOffshore()
	{
		print(A33);
		print(C33);
		print(naturalFrequency);
		print(period);
	}
};