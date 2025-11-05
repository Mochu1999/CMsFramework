#include "OffshoreIncludes.hpp"


int main(void)
{
	GlobalVariables gv;
	gv.program = offshoreProgram;

	GLFWwindow* window = initialize();



	TimeStruct tm;

	Shader shader3D("resources/shaders/shader3D.shader");
	Shader shader2D("resources/shaders/shader2D.shader");
	Shader shader2D_Instanced("resources/shaders/shader2D_Instanced.shader");
	Shader shaderText("resources/shaders/shaderText.shader");
	Camera camera(window, shader3D, shader2D, shader2D_Instanced, shaderText, gv);




	Offshore offshore(shader3D, shader2D, shaderText, shader2D_Instanced, camera, gv, tm);

	SettingsOffshore settings(camera, gv);

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

			clearScreen(gv);




			offshore.draw();





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


