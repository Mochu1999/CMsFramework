#pragma once

#include "Common.hpp"
#include "ShapeRenderer.hpp"
#include "axis.hpp"
#include "CadCreation.hpp"
#include "Text.hpp"


struct MainOC
{
	Shader& shader3D;
	Shader& shaderText;
	Camera& camera;
	GlobalVariables& gv;

	Text text;
	CadCreation cadCreator;

	Axis axis;
	AxisOCC axisOCC;


	Sphere light;
	p3 lightPos = { 300,250,400 };
	matrix4x4 model3DMatrix = gv.identityMatrix;

	Lines3D lines;
	Polyhedra polygon;


	std::vector<p3> colors = {
	{1.0f, 1.0f, 1.0f},
	{1.0f, 0.5f, 0.0f},   // Orange
	{0.0f, 1.0f, 0.0f},   // Green
	{0.5f, 0.0f, 1.0f},   // Purple
	{0.0f, 0.0f, 1.0f},   // Blue
	{1.0f, 1.0f, 0.0f},   // Yellow
	{1.0f, 0.0f, 1.0f},   // Magenta
	{0.0f, 1.0f, 1.0f},   // Cyan
	{1.0f, 0.0f, 0.0f},   // Red
	{0.5f, 1.0f, 0.0f}    // Lime
	};

	ShapeRenderer render0, render1, render2, render3, render4,
		render5, render6, render7, render8, render9;

	ShapeRenderer* renders[10] = {
	&render0, &render1, &render2, &render3, &render4,
	&render5, &render6, &render7, &render8, &render9
	};

	int currentRender = -1;

	MainOC(Shader& shader3D_, Shader& shaderText_, Camera& camera_, GlobalVariables& gv_)
		:shader3D(shader3D_), shaderText(shaderText_), camera(camera_), gv(gv_), light(3)
		, axisOCC(shader3D, gv, gv.identityMatrix), axis(shader3D, gv),
		cadCreator(shader3D, camera, gv, renders[0]), text("resources/Glyphs/Helvetica/Helvetica.otf", 36)
		, render0(gv, shader3D), render1(gv, shader3D), render2(gv, shader3D), render3(gv, shader3D), render4(gv, shader3D)
		, render5(gv, shader3D), render6(gv, shader3D), render7(gv, shader3D), render8(gv, shader3D), render9(gv, shader3D)
	{


		//renders[0]->importStlOpenCascade();
		//currentRender++;

		///*renders[0]->addRectangle({ 0,0,0 }, { 5,0,5 });
		//renders[0]->extrudeFace(10);
		//renders[1]->addSphereShape({ 5,0,5 }, 8);*/
		///*TopoDS_Shape intermShape;*/
		////booleanUnion(c, a, b);
		////booleanIntersection(c, a, b);
		////booleanCut(c, a, b);



		////a.addCircle({ 0,0,0 }, 0);
		////a.addBoxShape({ 0, 0, 0 }, { 5, 3, 2 });
		////a.addSphereShape({ 0, 0, 0 }, 5);
		////a.addIGES("ingenaval.igs");
		//light.addSet(lightPos);

		//if (gv.program == telemetry)
		//{
		//	shader3D.bind();
		//	shader3D.setUniform("u_lightPos", lightPos);
		//}
	}

	void draw()
	{
		cadCreator.draw(*renders[currentRender]);

		//print(currentRender);


		shader3D.bind();
		shader3D.setUniform("u_Model", gv.modelMatrixOCC);
		if (gv.cameraMode == centered)
		{
			axisOCC.draw(gv.modelMatrixOCC);
		}

		axis.draw(gv.modelMatrixOCC);





		/*shader3D.bind(); //This is for light, it is still there as far as shaders are concerned but it's not being rendered
		shader3D.setUniform("u_Model", gv.identityMatrix);
		shader3D.setUniform("u_Color", 255.0f / 255.0f, 255.0f / 255.0f, 255.0f / 255.0f, 1);

		shader3D.setUniform("u_fragmentMode", 0);*/
		//light.draw();

		for (int i = 0; i <= currentRender; i++)
		{
			renders[i]->draw(colors[i]);
		}


		/*a.draw();
		b.draw({1,0,0});*/
		//c.draw();
		opaque();
	}





	void booleanUnion()
	{
		if (currentRender < 1) return;

		ShapeRenderer interm(gv, shader3D);

		interm.shape = BRepAlgoAPI_Fuse(renders[currentRender - 1]->shape, renders[currentRender]->shape);
		renders[currentRender - 1]->clear(); renders[currentRender]->clear();
		renders[currentRender - 1]->addShape(interm.shape);


	}

	void booleanIntersection()
	{
		if (currentRender < 1) return;

		ShapeRenderer interm(gv, shader3D);

		interm.shape = BRepAlgoAPI_Common(renders[currentRender - 1]->shape, renders[currentRender]->shape);
		renders[currentRender - 1]->clear(); renders[currentRender]->clear();
		renders[currentRender - 1]->addShape(interm.shape);
	}

	void booleanCut()
	{
		if (currentRender < 1) return;

		ShapeRenderer interm(gv, shader3D);

		interm.shape = BRepAlgoAPI_Cut(renders[currentRender - 1]->shape, renders[currentRender]->shape);
		renders[currentRender - 1]->clear(); renders[currentRender]->clear();
		renders[currentRender - 1]->addShape(interm.shape);
	}

	void removeRenderer()
	{
		if (currentRender < 0)
		{
			return;
		}
		print(currentRender);

		renders[currentRender]->clear();
		currentRender--;
	}
};