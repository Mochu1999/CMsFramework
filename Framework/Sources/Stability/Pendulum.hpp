#pragma once

struct Pendulum
{
	Shader& shader3D;
	Camera& camera;
	TimeStruct& tm;

	Polyhedra body;

	float& omegaStructure;
	float& xStructure;
	float& ax; //horizontal acceleration of the structure
	float& ay; //vertical acceleration of the structure

	float r = 0.75; //m
	float m = 7850 * 4 / 3 * PI * r * r * r; //kg //13871.2 kg
	float l = 2; //m
	float inertia = m * l * l; //puntual mass, kg*m^2 

	float theta = PI / 3;// rad
	float omega = 0;// rad/s
	float alpha = 0;// rad/s^2

	float g = 9.80665; //ECHAR A COMMON
	float b = 0; //damping

	float lambda = 1;

	float totalEnergy = 0;

	float naturalPeriod = 2 * PI * sqrtf(g / l); //small angles

	matrix4x4 model3DMatrix = camera.identityMatrix;

	Pendulum(Shader& shader3D_, Camera& camera_, TimeStruct& tm_, float& omegaStructure_, float& xStructure_, float& ax_, float& ay_)
		:tm(tm_), shader3D(shader3D_), camera(camera_), omegaStructure(omegaStructure_), ax(ax_), ay(ay_), xStructure(xStructure_)
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
		float bGen = 0.05; //kt^2/Rl

		float tBear = -(bBear / inertia) * omega;
		//tBear = 0;

		float tWeight = -(g / l) * sinf(theta);
		float tGen = -lambda * (bGen / inertia) * (omega - omegaStructure);
		tGen = 0;

		//relative velocity always try to be 0
		//if they share direction and structure is faster pendulum gains energy 
		//if they don't share directions 

		// ml^2*alpha = -mgl*sin(theta) - bBear*omega - bGen*(omegaPend - omegaStr)
		// alpha = -g/l*sin(theta) - bGen/inertia*(omegaPend - omegaStr)

		float tInertial = -(ay / l) * sinf(theta) - (ax / l) * cosf(theta);

		alpha = tWeight + tBear + tGen + tInertial;

	}

	

	void draw()
	{

		camera.rotate3DModelMatrix(model3DMatrix, degrees(theta), { 0,0,1 });
		print(xStructure);
		camera.translate3DModelMatrix(model3DMatrix, { xStructure,0,0 });
		shader3D.setUniform("u_Model", model3DMatrix);

		body.draw();
	}
};