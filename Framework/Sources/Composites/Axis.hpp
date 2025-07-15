#pragma once

#include "Spheres.hpp"

struct Axis {
	Shader& shader3D;
	GlobalVariables& gv;

	Lines3D xLine, yLine, zLine, xLine2, zLine2;

	Axis(Shader& shader3D_, GlobalVariables& gv_) :shader3D(shader3D_), gv(gv_)
	{
		xLine.addSet({ {-100,0,0},{100,0,0} });
		yLine.addSet({ {0,-100,0},{0,100,0} });
		zLine.addSet({ {0,0,-100},{0,0,100} });
		xLine2.addSet({ {0,0,0},{5,0,0} });
		zLine2.addSet({ {0,0,0},{0,0,5} });







	}
	void draw() 
	{
		shader3D.bind();
		shader3D.setUniform("u_Model", gv.identityMatrix);

		glLineWidth(2); //this is deprecated and platform dependent
		shader3D.setUniform("u_fragmentMode", 1);


		shader3D.setUniform("u_Color", 1.0, 0.0, 0.0, 1.0);
		xLine.draw();
		shader3D.setUniform("u_Color", 0.0, 1.0, 0.0, 1.0);
		yLine.draw();
		shader3D.setUniform("u_Color", 0.0, 0.0, 1.0, 1.0);
		zLine.draw();

		glLineWidth(1);

		shader3D.setUniform("u_Color", 1, 1, 1, 1.0);

	}
	void draw(matrix4x4& modelMatrix)
	{
		shader3D.bind();
		shader3D.setUniform("u_Model", modelMatrix);

		glLineWidth(2); //this is deprecated and platform dependent
		shader3D.setUniform("u_fragmentMode", 1);


		shader3D.setUniform("u_Color", 1.0, 0.0, 0.0, 1.0);
		xLine.draw();
		shader3D.setUniform("u_Color", 0.0, 1.0, 0.0, 1.0);
		yLine.draw();
		shader3D.setUniform("u_Color", 0.0, 0.0, 1.0, 1.0);
		zLine.draw();

		glLineWidth(1);

		shader3D.setUniform("u_Color", 1, 1, 1, 1.0);

	}
};

struct AxisOCC {
	Shader& shader3D;
	GlobalVariables& gv;
	matrix4x4& modelMatrix;
	Lines3D xLine, yLine, zLine, xLine2, zLine2;

	AxisOCC(Shader& shader3D_, GlobalVariables& gv_, matrix4x4& modelMatrix_) :shader3D(shader3D_), gv(gv_), modelMatrix(modelMatrix_)
	{
		xLine.addSet({ {-100,0,0},{100,0,0} });
		yLine.addSet({ {0,-100,0},{0,100,0} });
		zLine.addSet({ {0,0,-100},{0,0,100} });
		xLine2.addSet({ {0,0,0},{5,0,0} });
		zLine2.addSet({ {0,0,0},{0,0,5} });







	}
	//esto es una gv, no habría que meterla, pero lo dejo para que decidas si queremos matener modelMatrixOCC como gv
	void draw(matrix4x4& modelMatrixOCC) 
	{
		shader3D.bind();
		shader3D.setUniform("u_Model", gv.identityMatrix);

		glLineWidth(0.5);
		shader3D.setUniform("u_fragmentMode", 1);


		shader3D.setUniform("u_Color", 0.7, 0.7, 0.7, 1.0);
		xLine.draw();
		yLine.draw();
		zLine.draw();

		glLineWidth(1);

		shader3D.setUniform("u_Model", modelMatrixOCC);
	}
};