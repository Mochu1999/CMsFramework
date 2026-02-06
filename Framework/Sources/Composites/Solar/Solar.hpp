#pragma once

#include "Common.hpp"
#include "Body.hpp"
#include "Lines3D.hpp"


struct Solar
{
	Shader& shader3D;
	Shader& shader2D;
	Shader& shaderText;

	Camera& camera;
	GlobalVariables& gv;
	TimeStruct& tm;

	ThreadPool& tp;

	vector<Body> bodies;

	float G = 6.674e-11;

	float totalEnergy = 0;

	Polygons2D dataBox;
	Lines2D dataBoxOutline;
	Text dataText;

	matrix4x4 model3DMatrix = identityMatrix;

	vector<Lines3D> trajectoriesLines;


	Solar(Shader& shader3D_, Shader& shader2D_, Shader& shaderText_, Camera& camera_, GlobalVariables& gv_, TimeStruct& tm_, ThreadPool& tp_)
		: shader3D(shader3D_), shader2D(shader2D_), shaderText(shaderText_), camera(camera_), gv(gv_), tm(tm_)
		, dataText("resources/Glyphs/Helvetica/Helvetica.otf", 20), tp(tp_)
	{

		vector<p2> dataRectangle = createRoundedSquare(windowTotal - p2{ 300,100 }, windowTotal, 30);
		dataBoxOutline.addSet(dataRectangle);
		dataBox.addSet(dataRectangle);

		activateLight();

		//Sphere is not copy/move safe, so it won't survive reallocation. Reserve for all the bodies or they'll break
		bodies.reserve(100);
		addBody(7.5e18, 1000, { 0,0,0 }, { 0,0,0 });
		addBody(1e15, 100, { 10000,0,0 }, { 0,0,123 });
		addBody(1e15, 100, { 20000,0,0 }, { 0,0,158 });

		cancelingMomentumSun();
		calculateAccelerations();
		calculateTrajectories();
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
				p3 r = bodies[j].position - bodies[i].position;
				// a = G*m*r/|r^3|
				float r2 = dot3(r, r) + eps * eps; //Plummer softening
				float inv = 1.0 / sqrt(r2);
				float inv3 = inv * inv * inv;

				bodies[i].acceleration += G * bodies[j].mass * r * inv3;
				bodies[j].acceleration -= G * bodies[i].mass * r * inv3;
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
		calculateTrajectories();
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

		shader3D.setUniform("u_Model", identityMatrix);
		shader3D.setUniform("u_fragmentMode", 1);
		for (auto t : trajectoriesLines)
		{
			t.draw();
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
				float dist = magnitude3(bodies[j].position - bodies[i].position) + eps;
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
		shader2D.setUniform("u_Model", identityMatrix);
		shader2D.setUniform("u_Color", 40 / 255.0f, 40 / 255.0f, 40 / 255.0f, 1.0f);
		dataBox.draw();
		shader2D.setUniform("u_Color", 40.0f / 255.0f, 239.9f / 255.0f, 239.0f / 255.0f, 1);
		glLineWidth(3);
		dataBoxOutline.draw();
		glLineWidth(1);

		shaderText.bind();
		dataText.addDynamicText({
			{windowTotal - p2{280,60}, "Total Energy: ",totalEnergy}
			});
		shaderText.setUniform("u_Color", 1.0f, 1.0f, 1.0f);
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








	void calculateTrajectories()
	{
		const int steps = 2000;
		const int stride = 5;
		const float dt = tm.solarUpdateInterval * 1000;

		vector<vector<p3>> trajectories(bodies.size());

		struct State
		{
			p3 pos;
			p3 vel;
			p3 acc;
			float mass;
		};

		vector<State> s(bodies.size());

		// snapshot
		for (size_t i = 0; i < bodies.size(); ++i)
		{
			s[i].pos = bodies[i].position;
			s[i].vel = bodies[i].velocity;
			s[i].acc = bodies[i].acceleration;
			s[i].mass = bodies[i].mass;
		}

		for (int step = 0; step < steps; ++step)
		{
			// kick
			for (auto& b : s)
				b.vel += 0.5f * dt * b.acc;

			// drift
			for (auto& b : s)
				b.pos += dt * b.vel;

			// recompute accelerations
			for (auto& b : s)
				b.acc = { 0,0,0 };

			for (size_t i = 0; i < s.size(); ++i)
			{
				for (size_t j = i + 1; j < s.size(); ++j)
				{
					p3 r = s[j].pos - s[i].pos;
					float r2 = dot3(r, r) + eps * eps;
					float inv = 1.0f / sqrt(r2);
					float inv3 = inv * inv * inv;

					p3 a = G * r * inv3;
					s[i].acc += a * s[j].mass;
					s[j].acc -= a * s[i].mass;
				}
			}

			// kick
			for (auto& b : s)
				b.vel += 0.5f * dt * b.acc;

			// record
			if (step % stride == 0)
			{
				for (size_t i = 0; i < s.size(); ++i)
					trajectories[i].push_back(s[i].pos);
			}
		}
		trajectoriesLines.clear();
		for (size_t i = 0; i < bodies.size(); i++)
		{
			trajectoriesLines.emplace_back();
			trajectoriesLines.back().addSet(trajectories[i]);
		}
	}
};