#pragma once

#include "Spheres.hpp"

float c = 1;
float graf1Val = 0;
float cosPlot(float& c) 
{
	c += 2.5;
	return (1 * c * c / 10000) * cos(radians(c));
}

struct Light 
{

	Shader& shader3D;
	Camera& camera;
	Sphere light;

	std::array<float, 16> shipModel3DMatrix = camera.identityMatrix;


	Polyhedra stl;
	Polyhedra casco1, casco2, tapa1, tapa2, tapa3, caja, timon, helice, motor, mecha, arbots
		, foilPopa, foilEstribor, foildBabor, aux, solar1, solar2, water;

	float waterLength = 10;

	p3 shipHeave = { 0,0.65,0 }; float shipHeaveIncrease = 0.0006;
	//rotation for the full ship?
	float rudderAngle = 0; float rudderIncrease = 0.2;
	float shipScale = 1;//borrar
	float propellerAngle = 0, propellerIncrease = 0.3;
	float foilAftAngle = 0, foilAftIncrease = 0.3;
	float foilStarboardAngle = 0, foilStarboardIncrease = 0.3;
	float foilPortAngle = 0, foilPortIncrease = 0.3;

	p3 lightPos = { 30,25,40 };

	Light(Shader& shader3D_, Camera& camera_, GlobalVariables gv) :shader3D(shader3D_), camera(camera_), light(3)
	{

		casco1.addPolyhedra("casco1.bin");
		casco2.addPolyhedra("casco2.bin");
		tapa1.addPolyhedra("tapa1.bin");
		tapa2.addPolyhedra("tapa2.bin");
		caja.addPolyhedra("caja.bin");
		arbots.addPolyhedra("arbots.bin");
		aux.addPolyhedra("aux0.bin");

		timon.addPolyhedra("timon.bin");
		helice.addPolyhedra("helice.bin");
		////{0,-1.001,0},{0.195, -1.001,0}
		motor.addPolyhedra("motor.bin");
		mecha.addPolyhedra("mecha.bin");
		tapa3.addPolyhedra("tapa3.bin");
		foilPopa.addPolyhedra("foilAft.bin"); //tienes cosas para borrar como foilPopa
		////{-0.023, -1.090,0}

		foilEstribor.addPolyhedra("foilStarboard.bin");
		////{1.640,-1.089, 0.35}


		///*readSimplePolyhedra(stl, "foildBabor.bin");
		//for (auto& p : stl.positions)
		//{
		//	p -= {1.639, -1.090, -0.35};
		//}

		//writeSimplePolyhedra(stl, "foilPort.bin");*/
		foildBabor.addPolyhedra("foilPort.bin");
		////{1.639,-1.090,-0.35}

		solar1.addPolyhedra("solar1.bin");
		solar2.addPolyhedra("solar2.bin");


		water.positions = { {0,0,0},{waterLength,0,0},{0,0,waterLength},{waterLength,0,0},{0,0,waterLength},{waterLength,0,waterLength} };
		water.indices = { 0,1,2 ,3,4,5 };
		water.normals = { {0,1,-0},{0,1,-0},{0,1,-0},{0,1,-0},{0,1,-0},{0,1,-0} };
		//creo que no puedes hacer esto porque el buffer de indices se espera menor, si aun con esas no sale aumentar buffer normals a 6 elementos
		/*water.positions = { {0,0,0},{100,0,0},{0,0,100},{100,0,100} };
		water.indices = { 0,1,2 ,1,3,2 };
		water.normals = { {0,1,-0},{0,1,-0},{0,1,-0},{0,1,-0} };*/

		
		light.addSet(lightPos);
		if (gv.program == telemetry)
		{
			shader3D.bind();
			shader3D.setUniform("u_lightPos", lightPos);
		}
	}

	void draw() 
	{
		shader3D.bind();
		shader3D.setUniform("u_Model", camera.identityMatrix);
		shader3D.setUniform("u_fragmentMode", 1);
		light.draw();

		opaque();

		std::array<float, 16> rudderMatrix, propellerMatrix, foilAftMatrix, foilStarboardMatrix, foilPortMatrix, rp, rf;

		if (rudderAngle >= 20 || rudderAngle <= -20)
			rudderIncrease = -rudderIncrease;

		if (foilAftAngle >= 20 || foilAftAngle <= -20)
			foilAftIncrease = -foilAftIncrease;

		if (foilStarboardAngle >= 20 || foilStarboardAngle <= -20)
			foilStarboardIncrease = -foilStarboardIncrease;

		if (foilPortAngle >= 20 || foilPortAngle <= -20)
			foilPortIncrease = -foilPortIncrease;

		if (shipHeave.y >= 0.7 || shipHeave.y <= 0.6)
			shipHeaveIncrease = -shipHeaveIncrease;

		rudderAngle += rudderIncrease;
		propellerAngle += 23;
		foilAftAngle += foilAftIncrease;
		foilStarboardAngle += foilStarboardIncrease;
		foilPortAngle += foilPortIncrease;
		shipHeave.y += shipHeaveIncrease;

		//u_model is being always changed to shipModel3DMatrix, but shipModel3DMatrix is only being created if input is changed

		//SOBRAN UN COJON DE OPERACIONES Y LIMPIAR
		camera.translate3DModelMatrix(shipModel3DMatrix, shipHeave);
		shader3D.setUniform("u_Model", shipModel3DMatrix); //borrar esta del inicio, solo necesitas la del final


		shader3D.setUniform("u_fragmentMode", 0); //illumination

		shader3D.setUniform("u_Color", 0.0f / 255.0f, 127.0f / 255.0f, 0.0f / 255.0f, 1.0f);
		casco1.stlDraw();
		casco2.stlDraw();

		shader3D.setUniform("u_Color", 137.0f / 255.0f, 18.0f / 255.0f, 18.0f / 255.0f, 1.0f);
		tapa1.stlDraw();
		tapa2.stlDraw();

		shader3D.setUniform("u_Color", 0.0f / 255.0f, 63.0f / 255.0f, 200.0f / 255.0f, 1.0f);
		caja.stlDraw();

		shader3D.setUniform("u_Color", 113.0f / 255.0f, 10.0f / 255.0f, 87.0f / 255.0f, 1.0f);
		arbots.stlDraw();

		shader3D.setUniform("u_Color", 255 / 255.0f, 0.0f / 255.0f, 0.0f / 255.0f, 1.0f);
		aux.stlDraw();
		mecha.stlDraw();




		//Timon
		{
			shipModel3DMatrix = camera.identityMatrix;

			camera.rotate3DModelMatrix(rudderMatrix, rudderAngle, { 0,1,0 });
			camera.translate3DModelMatrix(rudderMatrix, shipHeave);
			shader3D.setUniform("u_Model", rudderMatrix);


			shader3D.setUniform("u_Color", 113.0f / 255.0f, 10.0f / 255.0f, 87.0f / 255.0f, 1.0f);
			timon.stlDraw();

			shader3D.setUniform("u_Color", 0.0f / 255.0f, 63.0f / 255.0f, 200.0f / 255.0f, 1.0f);
			motor.stlDraw();

			shader3D.setUniform("u_Color", 137.0f / 255.0f, 18.0f / 255.0f, 18.0f / 255.0f, 1.0f);
			tapa3.stlDraw();

			camera.rotate3DModelMatrix(propellerMatrix, propellerAngle, { 1,0,0 });
			rp = multiplyMatrices(rudderMatrix, propellerMatrix);

			p3 propellerTranslation = { 0, -1.001,0 };
			propellerTranslation.x += 0.195 * cos(radians(rudderAngle));
			propellerTranslation.z -= 0.195 * sin(radians(rudderAngle));
			camera.translate3DModelMatrix(rp, propellerTranslation + shipHeave);
			shader3D.setUniform("u_Model", rp);
			shader3D.setUniform("u_Color", 255.0f / 255.0f, 255.0f / 255.0f, 255.0f / 255.0f, 1);
			helice.stlDraw();

			camera.rotate3DModelMatrix(foilAftMatrix, foilAftAngle, { 0,0,1 });
			rf = multiplyMatrices(rudderMatrix, foilAftMatrix);
			p3 foilAftTranslation = { 0, -1.090,0 };
			foilAftTranslation.x -= 0.023 * cos(radians(rudderAngle));
			foilAftTranslation.z += 0.023 * sin(radians(rudderAngle));
			camera.translate3DModelMatrix(rf, foilAftTranslation + shipHeave);
			shader3D.setUniform("u_Model", rf);

			shader3D.setUniform("u_Color", 16.0f / 255.0f, 28.0f / 255.0f, 82.0f / 255.0f, 1.0f);
			foilPopa.stlDraw();



			shader3D.setUniform("u_Model", camera.identityMatrix);

		}




		camera.rotate3DModelMatrix(foilStarboardMatrix, foilStarboardAngle, { 0,0,1 });
		p3 foilStarboardTranslation = { 1.640,-1.089, 0.35 };
		camera.translate3DModelMatrix(foilStarboardMatrix, foilStarboardTranslation + shipHeave);
		shader3D.setUniform("u_Model", foilStarboardMatrix);
		shader3D.setUniform("u_Color", 16.0f / 255.0f, 28.0f / 255.0f, 82.0f / 255.0f, 1.0f);
		foilEstribor.stlDraw();

		camera.rotate3DModelMatrix(foilPortMatrix, foilPortAngle, { 0,0,1 });
		p3 foilPortTranslation = { 1.640,-1.089, -0.35 };
		camera.translate3DModelMatrix(foilPortMatrix, foilPortTranslation + shipHeave);
		shader3D.setUniform("u_Model", foilPortMatrix);
		shader3D.setUniform("u_Color", 16.0f / 255.0f, 28.0f / 255.0f, 82.0f / 255.0f, 1.0f);
		foildBabor.stlDraw();


		camera.translate3DModelMatrix(shipModel3DMatrix, shipHeave);
		shader3D.setUniform("u_Model", shipModel3DMatrix);
		transparent();
		shader3D.setUniform("u_Color", 255.0f / 255.0f, 255.0f / 255.0f, 255.0f / 255.0f, 0.5f);
		solar1.stlDraw();
		shader3D.setUniform("u_Color", 0 / 255.0f, 0 / 255.0f, 0 / 255.0f, 1.0f);
		solar2.stlDraw();


		shader3D.setUniform("u_Model", camera.identityMatrix);
		camera.translate3DModelMatrix(shipModel3DMatrix, { -waterLength / 2,0,-waterLength / 2 });
		shader3D.setUniform("u_Model", shipModel3DMatrix);

		shader3D.setUniform("u_Color", 40.0f / 255.0f, 100.0f / 255.0f, 255.0f / 255.0f, 0.15f);
		shader3D.setUniform("u_Color", 40.0f / 255.0f, 189.9f / 255.0f, 255.0f / 255.0f, 0.2);
		water.stlDraw();

		opaque();


		shader3D.setUniform("u_Model", camera.identityMatrix);




	}

	void activateLight()
	{
		shader3D.bind();
		shader3D.setUniform("u_lightPos", lightPos);
	}
};