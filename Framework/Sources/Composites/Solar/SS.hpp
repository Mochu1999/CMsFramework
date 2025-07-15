#pragma once

#include "Common.hpp"
#include "Common.hpp"


void movePlanet(p3& planetPos){}

struct SS 
{

	Shader& shader3D;
	Camera& camera;
	GlobalVariables& gv;

	Sphere sun, planet1;

	p3 sunPos = { 0,0,0 };
	p3 planet1Pos = { 4,0,0 };
	std::array<float, 16> model3DMatrix = camera.identityMatrix;

	SS(Shader& shader3D_, Camera& camera_, GlobalVariables& gv_) : shader3D(shader3D_), camera(camera_), gv(gv_)
		, sun(1,1000),planet1(0.25,1000)
	{

		
		sun.addSet(sunPos);
		activateLight();

		planet1.addSet({ 4,0,0 });


	}

	void draw() 
	{
		shader3D.bind();
		shader3D.setUniform("u_Model", camera.identityMatrix);
		shader3D.setUniform("u_fragmentMode", 1);

		shader3D.setUniform("u_fragmentMode", 1);
		shader3D.setUniform("u_Color", 249/ 255.0f, 215 / 255.0f, 28 / 255.0f, 1.0f);
		sun.draw();

		opaque();
		camera.translate3DModelMatrix(model3DMatrix, planet1Pos);
		shader3D.setUniform("u_Model", model3DMatrix);

		shader3D.setUniform("u_fragmentMode", 0);
		shader3D.setUniform("u_Color", 1, 0, 0, 1);
		planet1.draw();

	}

	void activateLight()
	{
		if (gv.program == solar)
		{
			shader3D.bind();
			shader3D.setUniform("u_lightPos", sunPos);
		}
	}
};