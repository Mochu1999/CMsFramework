
#include "MainIncludes.hpp"


int main(void)
{
	GlobalVariables gv;

	GLFWwindow* window = initialize();



	TimeStruct tm;

	Shader shader3D("resources/shaders/shader3D.shader");
	Shader shader2D("resources/shaders/shader2D.shader");
	Shader shader2D_Instanced("resources/shaders/shader2D_Instanced.shader");
	Shader shaderText("resources/shaders/shaderText.shader");

	//main reason to add all the shaders there is to initialize the associated matrices in an encapsulated way
		// IT ISN'T REASONABLE TO HAVE THE SHADER INITIALIZATION IN CAMERA, ENCAPSULATE THEM ELSEWHERE
	Camera camera(window, shader3D, shader2D, shader2D_Instanced, shaderText, gv);


	Polyhedra finalTust;
	finalTust.addPolyhedra("finalTust.bin");
	Sphere tustin(3);
	tustin.addSet({ 12,0,12 });



	Text text("resources/Glyphs/Helvetica/Helvetica.otf", 36);
	text.addText({ {{ 10,950 }, tm.fps, " fps"},{{10,1000},tm.currentTime, " s"} });
	Text textAux("resources/Glyphs/Helvetica/Helvetica.otf", 48);


	Lines2D centerCross;
	centerCross.addSet({
		{gv.centerWindow.x - 20,gv.centerWindow.y},{gv.centerWindow.x + 20,gv.centerWindow.y},
		{gv.centerWindow.x,gv.centerWindow.y - 20},{gv.centerWindow.x,gv.centerWindow.y + 20} });
	centerCross.indices = { 0,1,2,3 };



	//Axis axis(shader3D, gv);
	Light ship(shader3D, camera, gv);
	SS ss(shader3D, camera, gv);

	Overlay2D overlay(shader2D, camera);
	Graphic graphic(shader2D, shader2D_Instanced, shaderText, camera, ship, tm, "A*cos(x)", { 1400,100 }, graf1Val);
	Graphic graphic2(shader2D, shader2D_Instanced, shaderText, camera, ship, tm, "rudderAngle", { 1400,400 }, ship.rudderAngle);
	ProgressBar pb(shader2D, shader2D_Instanced, shaderText, camera, ship, tm, { 1400 - 50,700 });

	World world(shader2D, shaderText, shader2D_Instanced, camera, gv, tm);
	MainOC mainOC(shader3D, shaderText, camera, gv);

	Settings settings(camera, gv, world);


	AllPointers allPointers(&camera, &gv, &world, &ship, &mainOC);
	glfwSetWindowUserPointer(window, &allPointers);
	glfwSetKeyCallback(window, keyboardEventCallback);
	glfwSetMouseButtonCallback(window, mouseEventCallback);
	glfwSetScrollCallback(window, scrollCallback);


	int counter = 0;

	//system("cls");
	while (!glfwWindowShouldClose(window))
	{
		getPos(window, gv.mPos);
		if (gv.isRunning)
		{
			tm.update();

			clearScreen(gv);




			switch (gv.program)
			{
			case telemetry:
				//axis.draw();
				ship.draw();


				overlay.draw();

				graf1Val = cosPlot(c);
				graphic.draw();
				graphic2.draw();
				pb.draw();

				break;

			case MRS:
				world.draw();
				break;

			case solar:
				//axis.draw();
				ss.draw();

				break;

			case openCascade:
				//axis.draw();
				mainOC.draw();
				finalTust.draw();
				tustin.draw();
				break;
			}




			//text
			transparent();
			shaderText.bind();
			text.draw();
			text.substituteText(0, { { 10,950 }, round2d(tm.fps), " fps" });
			text.substituteText(1, round1d(tm.currentTime), " s");
			p2 algo = { 0,0 };
			if (gv.cadMode == polyline)
			{
				textAux.addDynamicText({ {{ 10,700 }, "CAD mode: Polyline"}, });
				textAux.draw();
			}
			else if (gv.cadMode == rectangle)
			{
				textAux.addDynamicText({ {{ 10,700 }, "CAD mode: Rectangle"}, });
				textAux.draw();
			}
			else if (gv.cadMode == circle)
			{
				textAux.addDynamicText({ {{ 10,700 }, "CAD mode: Circle"}, });
				textAux.draw();
			}

			else if (gv.cadMode == sphere)
			{
				textAux.addDynamicText({ {{ 10,700 }, "CAD mode: Sphere"}, });
				textAux.draw();
			}
			else if (gv.cadMode == extrusion)
			{
				textAux.addDynamicText({ {{ 10,700 }, "CAD mode: Extrusion"}, });
				textAux.draw();
			}

			transparent();
			shader2D.bind();
			shader2D.setUniform("u_Model", camera.identityMatrix);
			shader2D.setUniform("u_Color", 1, 1, 1, 0.5);
			glLineWidth(2);
			centerCross.draw();
			glLineWidth(1);
			opaque();

			keyboardRealTimePolls(window, gv, camera, world);
			camera.updateCamera();



			//break;

		}
		//isRunning = false;


		glfwSwapBuffers(window);
		glfwPollEvents();

	}
	settings.write();
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}


