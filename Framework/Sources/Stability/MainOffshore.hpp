#pragma once

#include "Common.hpp"

struct Offshore
{
	float rho = 1025, g = 9.81;

	float m;

	float A33; //added mass
	float C33; //restoring coefficient

	float naturalFrequency, period;

	Offshore()
	{
		float D = 20, T = 80, H = 2;
		

		float area = PI * D * D / 4;

		m = area * T * rho;

		C33 = rho * g * area;

		A33 = (2.0 / 3 * PI * D * D * D / 8) * rho; //Volume of the semi sphere

		naturalFrequency = pow(C33 / (m + A33), 0.5);

		period = 2 * PI / naturalFrequency;
	
		printOffshore();
	}

	void printOffshore()
	{
		print(A33);
		print(C33);
		print(naturalFrequency);
		print(period);
	}
};