#pragma once

#include "Common.hpp"
#include "Body.hpp"




struct Solar
{
	Shader& shader3D;
	Camera& camera;
	GlobalVariables& gv;
	TimeStruct& tm;

	Body sun,planet1;


	float G = 1;


	matrix4x4 model3DMatrix = camera.identityMatrix;

	Solar(Shader& shader3D_, Camera& camera_, GlobalVariables& gv_, TimeStruct& tm_)
		: shader3D(shader3D_), camera(camera_), gv(gv_), tm(tm_)
		, sun(10e+4, 2), planet1(1, 0.5, { 25,0,0 }, {0,0,50})
	{
		if (gv.program == solarProgram) //an if because all other programs use other u_lightPos
			activateLight();



	}

	void update()
	{
		while (tm.counterUpdateSolar > 0)
		{
			float dt = tm.solarUpdateInterval;

			// Kick
			sun.velocity += 0.5f * sun.acceleration;
			planet1.velocity += 0.5f * dt * planet1.acceleration;

			// Drift
			sun.position += dt * sun.velocity;
			planet1.position += dt * planet1.velocity;

			// Recompute accelerations
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
	}

	void draw()
	{
		update();

		opaque();

		shader3D.bind();
		shader3D.setUniform("u_fragmentMode", 1);

		activateLight(); //in draw because I am expecting the sun pos to change
		camera.translate3DModelMatrix(model3DMatrix, sun.position);
		shader3D.setUniform("u_Model", model3DMatrix);

		shader3D.setUniform("u_Color", 249 / 255.0f, 215 / 255.0f, 28 / 255.0f, 1.0f);
		sun.draw();


		camera.translate3DModelMatrix(model3DMatrix, planet1.position);
		shader3D.setUniform("u_Model", model3DMatrix);

		shader3D.setUniform("u_fragmentMode", 0);
		shader3D.setUniform("u_Color", 1, 0, 0, 1);
		planet1.draw();

	}



	void activateLight()
	{
		shader3D.bind();
		shader3D.setUniform("u_lightPos", sun.position);
	}
};