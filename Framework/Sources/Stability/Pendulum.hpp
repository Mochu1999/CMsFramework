#pragma once
#include "Buoy.hpp"

struct Pendulum
{
	Polyhedra body;

	Buoy& buoy;

	float r = 0.75; //m
	float m = 7850 * 4 / 3 * PI * r * r * r; //kg //13871.2 kg
	float l = 2; //m
	float inertia = m * l * l; //puntual mass, kg*m^2 

	float theta =  PI *2/ 3;// rad
	float omega = 0;// rad/s
	float alpha = 0;// rad/s^2

	float b = 0; //damping

	float lambda = 1;

	float totalEnergy = 0;

	float naturalPeriod = 2 * PI * sqrtf(g / l); //small angles


	Pendulum(Buoy& buoy_)
		:buoy(buoy_)
	{
		Polyhedra stl;
		/*readSTL(stl, "pendulum2.stl");
		writeSimplePolyhedra(stl, "pendulum2.bin");*/

		body.addPolyhedra("pendulum2.bin");
	}

	void computePendulumEnergy()
	{
		float kinetic = 0.5f * m * (l * omega) * (l * omega);
		float potential = m * g * l * (1.0f - cosf(theta));
		totalEnergy = kinetic + potential;
	}
	void calculatePendulumAcceleration()
	{
		// estimated bearings damping coefficient
		float zeta = 0.05f; //it's a percentage
		float bBear = 2.0f * zeta * m * l * l * sqrtf(g / l);
		//bBear = 0;
		float bGen = 0.0; //kt^2/Rl

		float tBear = -(bBear / inertia) * omega;
		tBear = 0;

		float tWeight = -(g / l) * sinf(theta);
		float tGen = -lambda * (bGen / inertia) * (omega - buoy.omega);
		tGen = 0;

		//relative velocity always try to be 0
		//if they share direction and structure is faster pendulum gains energy 
		//if they don't share directions 

		float tInertial = -(buoy.ay / l) * sinf(theta) - (buoy.ax / l) * cosf(theta);

		alpha = tWeight + tBear +tGen + tInertial;

	}

	

	
};