#pragma once

#include "Common.hpp"
#include "Body.hpp"


//if press numpad when solarProgram focus on the body
//custom camera different from telemtry and such
//DIST CAN BE ZERO

struct Solar
{
	Shader& shader3D;
	Shader& shader2D;
	Shader& shaderText;

	Camera& camera;
	GlobalVariables& gv;
	TimeStruct& tm;


	vector<Body> bodies;

	float G = 6.674e-11;

	float totalEnergy = 0;

	Polygons2D dataBox;
	Lines2D dataBoxOutline;
	Text dataText;

	matrix4x4 model3DMatrix = camera.identityMatrix;


	Solar(Shader& shader3D_, Shader& shader2D_, Shader& shaderText_, Camera& camera_, GlobalVariables& gv_, TimeStruct& tm_)
		: shader3D(shader3D_), shader2D(shader2D_), shaderText(shaderText_), camera(camera_), gv(gv_), tm(tm_)
		, dataText("resources/Glyphs/Helvetica/Helvetica.otf", 20)
	{

		vector<p2> dataRectangle = createRoundedSquare(windowTotal-p2{300,100}, windowTotal, 30);
		dataBoxOutline.addSet(dataRectangle);
		dataBox.addSet(dataRectangle);

		if (gv.program == solarProgram) //an if because all other programs use other u_lightPos
			activateLight();

		//Sphere is not copy/move safe, so it won't survive reallocation. Reserve for all the bodies or they'll break
		bodies.reserve(100);
		addBody(7.5e10, 2, { 0,0,0 }, { 0,0,0 });
		addBody(1e6, 0.5, { 5,0,0 }, { 0,0,1 });
		//addBody(1000, 0.5, { 25,0,0 }, { 0,0,-15 });

		cancelingMomentumSun();
		calculateAccelerations();
	}

	void addBody(float mass, float radius, p3 position = { 0,0,0 }, p3 velocity = { 0,0,0 })
	{
		bodies.emplace_back(mass, radius, position, velocity);
	}

	void calculateAccelerations()
	{
		for (Body& b : bodies)
			b.acceleration = { 0,0,0 };

		for (size_t i = 0; i < bodies.size(); ++i)
		{
			for (size_t j = i + 1; j < bodies.size(); ++j)
			{
				// vector from i to j
				p3 r = bodies[j].position - bodies[i].position;
				float dist = magnitude3(r);
				p3 dir = normalize3(r);

				bodies[i].acceleration += G * bodies[j].mass * dir / (dist * dist);
				bodies[j].acceleration += -G * bodies[i].mass * dir / (dist * dist);
			}
		}
	}

	//velocity verlet, kick-drift-kick or leapfrog
	void update()
	{
		while (tm.counterUpdateSolar > 0)
		{
			float dt = tm.solarUpdateInterval;


			//first kick
			for (Body& b : bodies)
				b.velocity += 0.5f * dt * b.acceleration;

			//Drift
			//Positions use velocity evaluated at the half-step
			for (Body& b : bodies)
				b.position += dt * b.velocity;

			calculateAccelerations();

			// Kick
			for (Body& b : bodies)
				b.velocity += 0.5f * dt * b.acceleration;

			tm.counterUpdateSolar--;
		}

		computeTotalEnergy();
	}




	void draw()
	{
		update();

		opaque();

		shader3D.bind();
		shader3D.setUniform("u_fragmentMode", 1);

		activateLight(); //in draw because I am expecting the sun pos to change
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

		drawData();
	}


	void computeTotalEnergy()
	{
		float kinetic = 0.0f;
		float potential = 0.0f;

		// kinetic
		for (const Body& b : bodies)
		{
			kinetic += 0.5f * b.mass * dot3(b.velocity, b.velocity);
		}

		// potential
		for (size_t i = 0; i < bodies.size(); ++i)
		{
			for (size_t j = i + 1; j < bodies.size(); ++j)
			{
				float dist = magnitude3(bodies[j].position - bodies[i].position);
				potential += -G * bodies[i].mass * bodies[j].mass / dist;
			}
		}

		totalEnergy = kinetic + potential;
	}

	//We set a initial velocity to the sun so it keeps the barycenter fixed
	void cancelingMomentumSun()
	{
		if (bodies.empty()) return;

		p3 totalMomentum = { 0,0,0 };
		for (const Body& b : bodies)
			totalMomentum += b.mass * b.velocity;


		bodies[0].velocity -= totalMomentum / bodies[0].mass;
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
			{windowTotal-p2{280,60}, "Total Energy: ",totalEnergy}
			});
		dataText.draw();

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