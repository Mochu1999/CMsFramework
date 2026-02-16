#pragma once


#include "Common.hpp"
#include "Camera.hpp"


//Only have a single KeyMouseInputs included, the functions names are repeated!

//Pointers can be re-seated while references cannot. But we are not re seating anything so whatever
struct AllPointers {
	Camera* camera;
	GlobalVariables* gv;


	AllPointers(Camera* camera_, GlobalVariables* gv_)
		:camera(camera_), gv(gv_)
	{
	}
};

//The standard is to use callbacks for one-time event (typing, increase something once per press) and another function
//  for per-frame polling that updates each frame or based on deltaTime for smooth changes in a variable over frames


void keyboardEventCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {

	AllPointers* allPointers = static_cast<AllPointers*>(glfwGetWindowUserPointer(window));
	Camera* camera = allPointers->camera;
	GlobalVariables* gv = allPointers->gv;


	if (action == GLFW_PRESS)
	{
		//CTRL
		if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
		{


		}
		else
		{
			switch (key)
			{
			case GLFW_KEY_P:
				gv->isRunning = !gv->isRunning;
				print(gv->isRunning);
				break;
				/*case GLFW_KEY_C:
					keyCounter++;

					break;*/
			case GLFW_KEY_Q:

				break;

				//cameraModes //updateCamera in camera.cpp also need to be updated if ths is to change
			case GLFW_KEY_X:

				gv->cameraMode = drag;
				glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

				break;
			case GLFW_KEY_C:

				gv->cameraMode = FPS;
				gv->LastLMPos = gv->mPos;
				glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

				break;
			case GLFW_KEY_V:

				gv->cameraMode = centered;
				glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);


				break;

			}

		}

	}
}



//keys functions gets triggered once per frame
void keyboardRealTimePolls(GLFWwindow* window, GlobalVariables& gv, Camera& camera) {

	//The rest of the logic is in updateCamera

	// Rotation

	if (gv.cameraMode == drag || gv.cameraMode == FPS)
	{
		if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
			camera.calculateForward(camera.forward, camera.rotationSpeed, camera.right);

		if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
			camera.calculateForward(camera.forward, -camera.rotationSpeed, camera.right);

		if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
			camera.calculateForward(camera.forward, -camera.rotationSpeed, camera.up);

		if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
			camera.calculateForward(camera.forward, camera.rotationSpeed, camera.up);


		//translation
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
			camera.cameraPos = camera.cameraPos + camera.forward * camera.translationSpeed;

		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			camera.cameraPos = camera.cameraPos - camera.forward * camera.translationSpeed;

		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			camera.cameraPos = camera.cameraPos - camera.right * camera.translationSpeed;

		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			camera.cameraPos = camera.cameraPos + camera.right * camera.translationSpeed;

		if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
			camera.cameraPos.y += camera.translationSpeed;

		if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
			camera.cameraPos.y -= camera.translationSpeed;
	}




}



void getPos(GLFWwindow* window, p2& mPos) {
	double xpos1, ypos1;
	glfwGetCursorPos(window, &xpos1, &ypos1);

	mPos = { (float)xpos1,(float)(windowHeight - ypos1) };
}

void mouseEventCallback(GLFWwindow* window, int button, int action, int mods) {
	AllPointers* allPointers = static_cast<AllPointers*>(glfwGetWindowUserPointer(window));
	GlobalVariables* gv = allPointers->gv;
	Camera* camera = allPointers->camera;

	//LEFT
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		gv->isLmbPressed = 1;
		gv->LastLMPos = gv->mPos;


	}
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
	{
		gv->isLmbPressed = 0;
	}

	//RIGHT
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
	{


	}


	//MIDDLE
	if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS)
	{
		gv->isMmbPressed = 1;
		gv->LastMMPos = gv->mPos;
	}
	if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_RELEASE)
	{
		gv->isMmbPressed = 0;
	}
}


float scrollTranslationSpeedFactor = 4000;

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
	AllPointers* allPointers = static_cast<AllPointers*>(glfwGetWindowUserPointer(window));
	Camera* camera = allPointers->camera;
	GlobalVariables* gv = allPointers->gv;


	if (yoffset > 0)
	{

		camera->cameraPos = camera->cameraPos + camera->forward * camera->translationSpeed * scrollTranslationSpeedFactor;

	}
	else if (yoffset < 0)
	{

		camera->cameraPos = camera->cameraPos - camera->forward * camera->translationSpeed * scrollTranslationSpeedFactor;

	}

}

