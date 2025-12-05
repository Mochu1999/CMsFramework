#pragma once


#include "Common.hpp"
#include "Camera.hpp"
#include "Anchor.hpp"

//Only have a single KeyMouseInputs included, the functions names are repeated!

//Pointers can be re-seated while references cannot. But we are not re seating anything so whatever
struct AllPointers {
	Camera* camera;
	GlobalVariables* gv;
	Anchor* anchor;

	AllPointers(Camera* camera_, GlobalVariables* gv_, Anchor* anchor_)
		:camera(camera_), gv(gv_), anchor(anchor_)
	{
	}
};

//The standard is to use callbacks for one-time event (typing, increase something once per press) and another function
//  for per-frame polling that updates each frame or based on deltaTime for smooth changes in a variable over frames


void keyboardEventCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {

	AllPointers* allPointers = static_cast<AllPointers*>(glfwGetWindowUserPointer(window));
	Camera* camera = allPointers->camera;
	GlobalVariables* gv = allPointers->gv;
	Anchor* anchor = allPointers->anchor;


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
			case GLFW_KEY_Q:
				if (!anchor->brainrot.running)
				{
					anchor->brainrot.resume();
					anchor->work.stop();
					anchor->pleasure.stop();

				}
				else
					anchor->brainrot.stop();

				break;
			case GLFW_KEY_W:
				if (!anchor->work.running)
				{
					anchor->brainrot.stop();
					anchor->work.resume();
					anchor->pleasure.stop();

				}
				else
					anchor->work.stop();

				break;
			case GLFW_KEY_E:
				if (!anchor->pleasure.running)
				{
					anchor->brainrot.stop();
					anchor->work.stop();
					anchor->pleasure.resume();

				}
				else
					anchor->pleasure.stop();

				break;

			}

		}

	}
}



//keys functions gets triggered once per frame
void keyboardRealTimePolls(GLFWwindow* window, GlobalVariables& gv, Camera& camera) {


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


float scrollTranslationSpeedFactor = 20;

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
	AllPointers* allPointers = static_cast<AllPointers*>(glfwGetWindowUserPointer(window));
	Camera* camera = allPointers->camera;
	GlobalVariables* gv = allPointers->gv;


	if (yoffset > 0)
	{



	}
	else if (yoffset < 0)
	{


	}

}


