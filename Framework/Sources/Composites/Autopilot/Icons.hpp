#pragma once
#include "Lines2D_Instanced.hpp"
struct IconLight 
{

	Shader& shader2D;
	Camera& camera;


	Polyhedra interm;
	Polygons2D mainHull, sideHull1, sideHull2, aux1, aux2, aux3;
	Lines2D auxa, auxb;

	IconLight(Shader& shader2D_, Camera& camera_) :shader2D(shader2D_), camera(camera_)
	{

		readSTL(interm, "C:/dev/Project-Raccoon/Project Raccoon/Resources/stl models/mainHull.stl");
		polyhedraTo2D(interm, mainHull);

		readSTL(interm, "C:/dev/Project-Raccoon/Project Raccoon/Resources/stl models/sideHull1.stl");
		polyhedraTo2D(interm, sideHull1);

		readSTL(interm, "C:/dev/Project-Raccoon/Project Raccoon/Resources/stl models/sideHull2.stl");
		polyhedraTo2D(interm, sideHull2);

		readSTL(interm, "C:/dev/Project-Raccoon/Project Raccoon/Resources/stl models/aux1.stl");
		polyhedraTo2D(interm, aux1);

		readSTL(interm, "C:/dev/Project-Raccoon/Project Raccoon/Resources/stl models/aux2.stl");
		polyhedraTo2D(interm, aux2);

		readSTL(interm, "C:/dev/Project-Raccoon/Project Raccoon/Resources/stl models/aux3.stl");
		polyhedraTo2D(interm, aux3);


		auxa.addSet({ {-5,0},{5,0} });
		auxb.addSet({ {0,-5},{0,5} });

	}

	void draw() 
	{
		

		shader2D.bind();
		/*std::array<float, 16> mapModel2DMatrix = camera.create2DModelMatrix({0,0}, 0, 50);
		shader2D.setUniform("u_Model", mapModel2DMatrix);*/

		shader2D.setUniform("u_Color", 1, 1, 0, 1);
		mainHull.draw();
		sideHull1.draw();
		sideHull2.draw();

		shader2D.setUniform("u_Color", 1, 1, 1, 1);
		aux1.draw();
		aux2.draw();
		aux3.draw();

		/*shader2D.setUniform("u_Color", 1, 0, 0, 1);
		glLineWidth(3);
		auxa.draw();
		auxb.draw();
		glLineWidth(1);*/
	}

};






struct ArrowsMRS
{
	Shader& shader2D;
	Shader& shader2D_Instanced;
	Camera& camera;

	//Lines2D arrow;
	Lines2D_Instanced lines;
	//Polygons2D polygon;

	vector<p2> positions = { {0,0},{0,-0.019},{0.88,-0.019},{0.714,-0.145},{0.742,-0.175},{1,0}
		, { 0.742,0.175 },{0.714,0.145},{0.88,0.019},{0,0.019},{0,0} };

	vector<unsigned int> indices = { 0,1,2 ,2,3,4 ,2,4,5, 0,2,5 ,5,6,7 ,5,7,8 ,5,8,0, 8,9,0 };

	ArrowsMRS(Shader& shader2D_, Shader& shader2D_Instanced_ , Camera& camera_) 
		:shader2D(shader2D_), shader2D_Instanced(shader2D_Instanced_), camera(camera_)
	{
		//arrow.addSet(positions);
		//polygon.addSet(positions,indices);

		lines.addSet(positions);
		lines.addInstances({ { {100,100},0,{50,50} } });

	}

	void update(const vector<InstanceAttributes>& currentInstancing)
	{
		lines.clear();
		lines.addSet(positions);
		lines.addInstances(currentInstancing);
	}

	void draw(std::array<float, 16>& mapModel2DMatrix)
	{


		//shader2D.bind();
		//std::array<float, 16> mapModel2DMatrix = camera.create2DModelMatrix({100,500}, 0, 500);
		//shader2D.setUniform("u_Model", mapModel2DMatrix);


		//shader2D.setUniform("u_Color", 1, 1, 1, 1);
		////arrow.draw();
		////polygon.draw();

		

		shader2D_Instanced.bind();


		//shader2D_Instanced.setUniform("u_Model", camera.identityMatrix);
		shader2D_Instanced.setUniform("u_Model", mapModel2DMatrix);
		shader2D.setUniform("u_Color", 1, 1, 1, 1);
		lines.draw();
	}
};