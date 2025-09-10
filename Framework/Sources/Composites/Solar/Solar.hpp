#pragma once

#include "Common.hpp"
#include "Body.hpp"




struct Solar
{
	Shader& shader3D;
	Camera& camera;
	GlobalVariables& gv;

	Body sun,planet1;


	float G = 1;


	matrix4x4 model3DMatrix = camera.identityMatrix;

	Solar(Shader& shader3D_, Camera& camera_, GlobalVariables& gv_) : shader3D(shader3D_), camera(camera_), gv(gv_)
		, sun(4, 2), planet1(1, 0.5, { 5,0,0 })
	{
		if (gv.program == solarProgram) //an if because all other programs use other u_lightPos
			activateLight();



	}

	void draw()
	{
		opaque();

		shader3D.bind();
		shader3D.setUniform("u_fragmentMode", 1);

		activateLight(); //in draw because I am expecting the sun pos to change
		camera.translate3DModelMatrix(model3DMatrix, sun.position);
		shader3D.setUniform("u_Model", model3DMatrix);

		shader3D.setUniform("u_Color", 249 / 255.0f, 215 / 255.0f, 28 / 255.0f, 1.0f);
		sun.draw();

		//posPlanet1 += {0, 0.1, 0};
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