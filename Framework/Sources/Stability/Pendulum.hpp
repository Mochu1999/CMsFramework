#pragma once
#include "Buoy.hpp"

struct Pendulum
{
	Polyhedra body;

	Buoy& buoy;

	float rext = 7.5; //m
	float rint = 5; //m
	float m = 616538; //kg 
	float inertia = 0.5f*m*(rext*rext-rint*rint); // kg*m^2 

	float theta = radians(0);// rad
	float omega = 0;// rad/s
	float alpha = 0.52;// rad/s^2

	float b = 0; //damping


	float totalEnergy = 0;


	float deltaTheta = 0;

	float genEnergy = 0;

	Pendulum(Buoy& buoy_)
		:buoy(buoy_)
	{
		/*Polyhedra stl;
		readSTL(stl, "InertiaWheel.stl");
		writeSimplePolyhedra(stl, "pendulum2.bin");*/

		body.addPolyhedra("pendulum2.bin");
	}

	void computePendulumEnergy()
	{
		float kinetic = 0; //= 0.5f * m * (l * omega) * (l * omega);
		float potential = 0;// = m * g * l * (1.0f - cosf(theta));
		totalEnergy = kinetic + potential;
	}
	void calculatePendulumAcceleration()
	{
		// estimated bearings damping coefficient
		float zeta = 0.05f; //it's a percentage
		float bBear = 2.0f * zeta;
		//bBear = 0;

		float tBear = -(bBear ) * omega; //ignoring the division bc the bGen is arbitrary
		tBear = 0;

		
		float bGen = 0; //kt^2/Rl
		float tGen = -buoy.lambda * bGen * (omega - buoy.omega);
		//tGen = 0;

		genEnergy = abs(tGen) * 0.98*9.81*1000;

		//relative velocity always try to be 0
		//if they share direction and structure is faster pendulum gains energy 
		//if they don't share directions 


		// I * alpha = tBear + tGen
		alpha = (tBear + tGen) / inertia;;
		alpha = 1;
	}
};














//struct Pendulum
//{
//	Polyhedra body;
//
//	Buoy& buoy;
//
//	float r = 1.75; //m
//	float m = 7850 * 4 / 3 * PI * r * r * r; //kg //13871.2 kg
//	float l = 2; //m
//	float inertia = m * l * l; //puntual mass, kg*m^2 
//
//	float theta = radians(10);// rad
//	float omega = 0;// rad/s
//	float alpha = 0;// rad/s^2
//
//	float b = 0; //damping
//
//	float tGen = 0;
//
//	float totalEnergy = 0;
//
//	float naturalPeriod = 2 * PI * sqrtf(g / l); //small angles
//
//	float deltaTheta = 0;
//
//	float genEnergy = 0;
//
//	Pendulum(Buoy& buoy_)
//		:buoy(buoy_)
//	{
//
//		/*Polyhedra stl;
//		readSTL(stl, "PendulumNew.stl");
//		writeSimplePolyhedra(stl, "pendulum2.bin");*/
//
//		body.addPolyhedra("pendulum2.bin");
//	}
//
//	void computePendulumEnergy()
//	{
//		float kinetic = 0.5f * m * (l * omega) * (l * omega);
//		float potential = m * g * l * (1.0f - cosf(theta));
//		totalEnergy = kinetic + potential;
//	}
//	void calculatePendulumAcceleration()
//	{
//		// estimated bearings damping coefficient
//		float zeta = 0.05f; //it's a percentage
//		float bBear = 2.0f * zeta * m * l * l * sqrtf(g / l);
//		//bBear = 0;
//
//		float tBear = -(bBear / inertia) * omega; //ignoring the division bc the bGen is arbitrary
//		tBear = 0;
//
//		float tWeight = -(g / l) * sinf(theta);
//
//		/*if (omega - buoy.omega < 0) buoy.lambda = 0;
//		else*/ buoy.lambda = 1;
//		float bGen = 1; //kt^2/Rl
//		tGen = -buoy.lambda * (bGen /*/ inertia*/) * (omega - buoy.omega);
//		//tGen = 0;
//		genEnergy = abs(tGen) * 0.98 * 9.81 * 1000;
//		//tGen = 0;
//		//print(tGen);
//		//print(buoy.lambda);
//		//tGen = 0;
//
//		//relative velocity always try to be 0
//		//if they share direction and structure is faster pendulum gains energy 
//		//if they don't share directions 
//
//		float tInertial = -(buoy.ay / l) * sinf(theta) - (buoy.ax / l) * cosf(theta);
//
//		alpha = tWeight + tBear + tGen + tInertial;
//
//	}
//
//
//
//
//};