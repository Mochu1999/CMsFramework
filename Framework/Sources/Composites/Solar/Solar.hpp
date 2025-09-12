#pragma once

#include "Common.hpp"
#include "Body.hpp"


//if press numpad when solarProgram focus on the body
//custom camera different from telemtry and such

struct Solar
{
	Shader& shader3D;
	Camera& camera;
	GlobalVariables& gv;
	TimeStruct& tm;

	vector<Body> bodies;


	float G = 1;


	matrix4x4 model3DMatrix = camera.identityMatrix;

	Solar(Shader& shader3D_, Camera& camera_, GlobalVariables& gv_, TimeStruct& tm_)
		: shader3D(shader3D_), camera(camera_), gv(gv_), tm(tm_)
	{
		if (gv.program == solarProgram) //an if because all other programs use other u_lightPos
			activateLight();

		addBody(10e+4, 2);
		addBody(1, 0.5, { 25,0,0 }, { 0,0,50 });

		update();
	}

	void addBody(float mass, float radius, p3 position = { 0,0,0 }, p3 velocity = { 0,0,0 }, p3 acceleration = { 0,0,0 })
	{
		Body interm(mass, radius, position, velocity, acceleration);
		bodies.push_back(interm);
	}

	/*void update()
	{
		while (tm.counterUpdateSolar > 0)
		{
			float dt = tm.solarUpdateInterval;


			//velocity verlet, kick-drift-kick
			// Kick
			sun.velocity += 0.5f * sun.acceleration;
			planet1.velocity += 0.5f * dt * planet1.acceleration;

			// Drift
			//Positions use velocity evaluated at the half-step
			sun.position += dt * sun.velocity;
			planet1.position += dt * planet1.velocity;

			// Recomputes accelerations
			p3 r = planet1.position - sun.position;
			float dist = magnitude3(r);
			p3 dir = normalize3(r);

			planet1.acceleration = -G * sun.mass * dir / (dist * dist);
			sun.acceleration = G * planet1.mass * dir / (dist * dist);

			// Kick
			sun.velocity += 0.5f * dt * sun.acceleration;
			planet1.velocity += 0.5f * dt * planet1.acceleration;

			tm.counterUpdateSolar--;
		}
	}*/

	void update()
	{
		while (tm.counterUpdateSolar > 0)
		{
			float dt = tm.solarUpdateInterval;

			//velocity verlet, kick-drift-kick
			//first kick
			for (Body& b : bodies)
				b.velocity += 0.5f * b.acceleration;

			//Drift
			//Positions use velocity evaluated at the half-step
			for (Body& b : bodies)
				b.position += dt * b.velocity;

			// Reset accelerations
			for (Body& b : bodies)
				b.acceleration = { 0,0,0 };

			for (size_t i = 0; i < bodies.size(); ++i)
			{
				for (size_t j = i + 1; j < bodies.size(); ++j)
				{
					//// vector from i to j
					//p3 r = bodies[j].position - bodies[i].position;
					//float dist = magnitude3(r);
					//p3 dir = normalize3(r);

					//// a_i = + G * m_j * dir / r^2    (toward j)
					//p3 a_i = G * bodies[j].mass * dir / (dist * dist);

					//// a_j = - G * m_i * dir / r^2    (toward i, opposite direction)
					//p3 a_j = -G * bodies[i].mass * dir / (dist * dist);

					//// add this pair’s contribution to each body
					//bodies[i].acceleration += a_i;
					//bodies[j].acceleration += a_j;
				}
			}

			// Kick
			for (Body& b : bodies)
				b.velocity += 0.5f * dt * b.acceleration;

			tm.counterUpdateSolar--;
		}
	}


	float computeTotalEnergy()
	{
		//// kinetic
		//float K_sun = 0.5f * sun.mass * dot(sun.velocity, sun.velocity);
		//float K_planet = 0.5f * planet1.mass * dot(planet1.velocity, planet1.velocity);

		//// potential
		//p3 r = planet1.position - sun.position;
		//float dist = magnitude3(r);
		//float U = -G * sun.mass * planet1.mass / dist;

		//return K_sun + K_planet + U;
	}


	void draw()
	{
		update();

		print(bodies[0].position);
		print(bodies[0].radius);
		print(bodies[0].mass);

		activateLight(); //in draw because I am expecting the sun pos to change
		
		opaque();

		shader3D.bind();
		shader3D.setUniform("u_fragmentMode", 1);

		camera.translate3DModelMatrix(model3DMatrix, bodies[0].position);
		shader3D.setUniform("u_Model", model3DMatrix);

		shader3D.setUniform("u_Color", 249 / 255.0f, 215 / 255.0f, 28 / 255.0f, 1.0f);
		bodies[0].draw();

		shader3D.setUniform("u_fragmentMode", 0);

		for (size_t i = 1; i < bodies.size(); i++)
		{
			camera.translate3DModelMatrix(model3DMatrix, bodies[i].position);
			shader3D.setUniform("u_Model", model3DMatrix);
			shader3D.setUniform("u_Color", 1, 0, 0, 1);

			bodies[i].draw();
		}


	}



	void activateLight()
	{
		//bodies[0] is always the sun
		if (bodies.size())
		{
			shader3D.bind();
			shader3D.setUniform("u_lightPos", bodies[0].position);
		}
	}
};