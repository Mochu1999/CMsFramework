#include "IncludesOptics.hpp"


int main(void)
{



	GlobalVariables gv;

	GLFWwindow* window = initialize();

	//ThreadPool tp;

	TimeStruct tm;

	Shader shader3D("resources/shaders/shader3D.shader");
	Shader shader2D("resources/shaders/shader2D.shader");
	Shader shader2D_Instanced("resources/shaders/shader2D_Instanced.shader");
	Shader shaderText("resources/shaders/shaderText.shader");
	Camera camera(window, shader3D, shader2D, shader2D_Instanced, shaderText, gv);


	Optics optics(shader2D,gv);
	Axis axis(shader3D, gv);


	SettingsOptics settings(camera, gv);

	AllPointers allPointers(&camera, &gv);
	glfwSetWindowUserPointer(window, &allPointers);
	glfwSetKeyCallback(window, keyboardEventCallback);
	glfwSetMouseButtonCallback(window, mouseEventCallback);
	glfwSetScrollCallback(window, scrollCallback);


	while (!glfwWindowShouldClose(window))
	{


		getPos(window, gv.mPos);
		if (gv.isRunning)
		{
			tm.update();

			opaque();
			glClearColor(0.035f, 0.065f, 0.085f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


			axis.draw();

			optics.draw();

			keyboardRealTimePolls(window, gv, camera);
			camera.updateCamera();
		}


		glfwSwapBuffers(window);
		glfwPollEvents();

	}
	settings.write();
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}