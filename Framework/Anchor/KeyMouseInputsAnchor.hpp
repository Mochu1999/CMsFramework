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

//only triggered when a mouse button is pressed or released
void mouseEventCallback(GLFWwindow* window, int button, int action, int mods) {
	AllPointers* allPointers = static_cast<AllPointers*>(glfwGetWindowUserPointer(window));
	Anchor* anchor = allPointers->anchor;

	//gets in what state are we 
	ButtonMode mode = anchor->ui.processMouseButton(button, action);
	if (mode == None) return;

	else if(mode == Close)
	{
		glfwSetWindowShouldClose(window, GLFW_TRUE);
		return;
	}

	else if (mode == Minimize)
	{
		glfwIconifyWindow(window);
		return;
	}

	anchor->mode = mode;
	anchor->logic();
	
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


