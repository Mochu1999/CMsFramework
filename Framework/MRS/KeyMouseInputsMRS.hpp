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


float scrollTranslationSpeedFactor = 20;

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





//#pragma once
//
//#include "Globe.hpp"
//#include "Autopilot.hpp"
//#include "Light.hpp"
//#include "solar.hpp"
//#include "MainOC.hpp"
//#include "Common.hpp"
//#include "Camera.hpp"
//
//
////Only have a single KeyMouseInputs included, the functions names are repeated!
//
////Pointers can be re-seated while references cannot. But we are not re seating anything so whatever
//struct AllPointers {
//	Camera* camera;
//	GlobalVariables* gv;
//	Autopilot* autopilot;
//	Light* ship;
//	Solar* solar;
//	MainOC* mainOC;
//
//	AllPointers(Camera* camera_, GlobalVariables* gv_, Autopilot* autopilot_, Light* ship_, MainOC* mainOC_)
//		:camera(camera_), autopilot(autopilot_), gv(gv_), ship(ship_), mainOC(mainOC_) {
//	}
//};
//
////The standard is to use callbacks for one-time event (typing, increase something once per press) and another function
////  for per-frame polling that updates each frame or based on deltaTime for smooth changes in a variable over frames
//
//
//void keyboardEventCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
//
//	AllPointers* allPointers = static_cast<AllPointers*>(glfwGetWindowUserPointer(window));
//	Camera* camera = allPointers->camera;
//	Autopilot* autopilot = allPointers->autopilot;
//	Light* ship = allPointers->ship;
//	GlobalVariables* gv = allPointers->gv;
//	MainOC* mainOC = allPointers->mainOC;
//	Solar* solar = allPointers->solar;
//
//
//	if (action == GLFW_PRESS)
//	{
//		//CTRL
//		if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
//		{
//			switch (key)
//			{
//			case GLFW_KEY_1:
//				gv->program = telemetry;
//				ship->activateLight();
//
//				break;
//			case GLFW_KEY_2:
//				gv->program = MRS;
//
//				break;
//			case GLFW_KEY_3:
//				gv->program = solarProgram;
//				//solar->activateLight();
//
//				break;
//			case GLFW_KEY_4:
//				gv->program = openCascade;
//
//				break;
//			case GLFW_KEY_5:
//				gv->program = offshoreProgram;
//
//				break;
//			case GLFW_KEY_O: //changing autopilots
//				if (gv->program == MRS)
//				{
//
//				}
//				if (gv->program == openCascade)
//				{
//					mainOC->renders[mainOC->currentRender]->openShape();
//				}
//				break;
//			case GLFW_KEY_S:
//				if (gv->program == MRS)
//				{
//				}
//				if (gv->program == openCascade)
//				{
//					mainOC->renders[mainOC->currentRender]->saveShape();
//				}
//				break;
//			}
//
//		}
//		else
//		{
//			switch (key)
//			{
//			case GLFW_KEY_P:
//				gv->isRunning = !gv->isRunning;
//				print(gv->isRunning);
//				break;
//				/*case GLFW_KEY_C:
//					keyCounter++;
//
//					break;*/
//			case GLFW_KEY_Q:
//				if (gv->program == 1)
//				{
//					if (autopilot->ui.show)
//						autopilot->ui.show = 0;
//					else
//						autopilot->ui.show = 1;
//				}
//				break;
//
//				//cameraModes //updateCamera in camera.cpp also need to be updated if ths is to change
//			case GLFW_KEY_X:
//				if (gv->program == telemetry || gv->program == solarProgram || gv->program == openCascade
//					|| gv->program == offshoreProgram)
//				{
//					gv->cameraMode = drag;
//					glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
//				}
//				break;
//			case GLFW_KEY_C:
//				if (gv->program == telemetry || gv->program == solarProgram || gv->program == openCascade
//					|| gv->program == offshoreProgram)
//				{
//					gv->cameraMode = FPS;
//					gv->LastLMPos = gv->mPos;
//					glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
//				}
//				break;
//			case GLFW_KEY_V:
//				if (gv->program == telemetry || gv->program == solarProgram || gv->program == openCascade
//					|| gv->program == offshoreProgram)
//				{
//					gv->cameraMode = centered;
//					glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
//
//				}
//				break;
//			case GLFW_KEY_O:
//				if (gv->program == openCascade)
//				{
//					gv->visualizationMode = triangulated;
//				}
//				break;
//			case GLFW_KEY_I:
//				if (gv->program == openCascade)
//				{
//					gv->visualizationMode = wire;
//
//
//				}
//				break;
//			case GLFW_KEY_T:
//				if (gv->program == openCascade)
//				{
//					if (gv->cadMode == polyline)
//					{
//						gv->cadMode = none;
//					}
//					else if (gv->cadMode != polyline)
//					{
//						gv->cadMode = polyline;
//						mainOC->currentRender++;
//					}
//				}
//				break;
//			case GLFW_KEY_Y:
//				if (gv->program == openCascade)
//				{
//					if (gv->cadMode == rectangle)
//					{
//						gv->cadMode = none;
//					}
//					else if (gv->cadMode != rectangle)
//					{
//						gv->cadMode = rectangle;
//						mainOC->currentRender++;
//					}
//				}
//				break;
//			case GLFW_KEY_G:
//				if (gv->program == openCascade)
//				{
//					if (gv->cadMode == circle)
//					{
//						gv->cadMode = none;
//					}
//					else if (gv->cadMode != circle)
//					{
//						gv->cadMode = circle;
//						mainOC->currentRender++;
//					}
//				}
//				break;
//			case GLFW_KEY_H:
//				if (gv->program == openCascade)
//				{
//					if (gv->cadMode == sphere)
//					{
//						gv->cadMode = none;
//					}
//					else if (gv->cadMode != sphere)
//					{
//						gv->cadMode = sphere;
//						mainOC->currentRender++;
//					}
//				}
//				break;
//			case GLFW_KEY_U:
//				if (gv->program == openCascade)
//				{
//					if (gv->cadMode == extrusion)
//					{
//						gv->cadMode = none;
//					}
//					else if (gv->cadMode != extrusion)
//					{
//
//						if (faceCounter(mainOC->renders[mainOC->currentRender]->shape) != 1) return;
//
//						gv->cadMode = extrusion;
//						camera->setCursorToXZPoint(mainOC->renders[mainOC->currentRender]->positions[0]);
//					}
//				}
//				break;
//			case GLFW_KEY_J:
//				if (gv->program == openCascade)
//				{
//					mainOC->booleanUnion();
//				}
//				break;
//			case GLFW_KEY_K:
//				if (gv->program == openCascade)
//				{
//					mainOC->booleanIntersection();
//				}
//				break;
//			case GLFW_KEY_L:
//				if (gv->program == openCascade)
//				{
//					mainOC->booleanCut();
//				}
//				break;
//			case GLFW_KEY_R:
//				if (gv->program == openCascade)
//				{
//					mainOC->removeRenderer();
//				}
//				break;
//			}
//
//		}
//
//	}
//}
//
//
//
////keys functions gets triggered once per frame
//void keyboardRealTimePolls(GLFWwindow* window, GlobalVariables& gv, Camera& camera, Autopilot& autopilot) {
//
//	//The rest of the logic is in updateCamera
//
//	// Rotation
//	if (gv.program == telemetry || gv.program == solarProgram || gv.program == openCascade || gv.program == offshoreProgram)
//	{
//		if (gv.cameraMode == drag || gv.cameraMode == FPS)
//		{
//			if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
//				camera.calculateForward(camera.forward, camera.rotationSpeed, camera.right);
//
//			if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
//				camera.calculateForward(camera.forward, -camera.rotationSpeed, camera.right);
//
//			if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
//				camera.calculateForward(camera.forward, -camera.rotationSpeed, camera.up);
//
//			if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
//				camera.calculateForward(camera.forward, camera.rotationSpeed, camera.up);
//
//
//			//translation
//			if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
//				camera.cameraPos = camera.cameraPos + camera.forward * camera.translationSpeed;
//
//			if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
//				camera.cameraPos = camera.cameraPos - camera.forward * camera.translationSpeed;
//
//			if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
//				camera.cameraPos = camera.cameraPos - camera.right * camera.translationSpeed;
//
//			if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
//				camera.cameraPos = camera.cameraPos + camera.right * camera.translationSpeed;
//
//			if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
//				camera.cameraPos.y += camera.translationSpeed;
//
//			if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
//				camera.cameraPos.y -= camera.translationSpeed;
//		}
//
//
//	}
//	else if (gv.program == MRS)
//	{
//		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
//			autopilot.world.translationFactor -= {0, 10};
//
//		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
//			autopilot.world.translationFactor += {0, 10};
//
//		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
//			autopilot.world.translationFactor += {10, 0};
//
//		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
//			autopilot.world.translationFactor -= {10, 0};
//	}
//}
//
//
//
//void getPos(GLFWwindow* window, p2& mPos) {
//	double xpos1, ypos1;
//	glfwGetCursorPos(window, &xpos1, &ypos1);
//
//	mPos = { (float)xpos1,(float)(windowHeight - ypos1) };
//}
//
//void mouseEventCallback(GLFWwindow* window, int button, int action, int mods) {
//	AllPointers* allPointers = static_cast<AllPointers*>(glfwGetWindowUserPointer(window));
//	GlobalVariables* gv = allPointers->gv;
//	Autopilot* autopilot = allPointers->autopilot;
//	MainOC* mainOC = allPointers->mainOC;
//	Camera* camera = allPointers->camera;
//
//	//LEFT
//	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
//	{
//		gv->isLmbPressed = 1;
//		gv->LastLMPos = gv->mPos;
//
//		if (gv->cadMode == polyline)
//		{
//			gv->cadPositions.back() = camera->cursorToXZPlane();
//			gv->cadPositions.push_back(camera->cursorToXZPlane());
//
//
//			if (gv->cadPositions.size() >= 2)
//			{
//				mainOC->cadCreator.lines.clear();
//				mainOC->cadCreator.lines.addSet(gv->cadPositions);
//
//			}
//		}
//		if (gv->cadMode == rectangle)
//		{
//			if (mainOC->cadCreator.isPoint1 == 0)
//			{
//				mainOC->cadCreator.point1 = camera->cursorToXZPlane();
//				mainOC->cadCreator.isPoint1 = 1;
//			}
//			else
//			{
//				mainOC->renders[mainOC->currentRender]->addRectangle(mainOC->cadCreator.point1, camera->cursorToXZPlane());
//				mainOC->cadCreator.isPoint1 = 0;
//				gv->cadMode = none;
//			}
//		}
//		if (gv->cadMode == circle)
//		{
//			if (mainOC->cadCreator.isPoint1 == 0)
//			{
//				mainOC->cadCreator.isPoint1 = 1;
//				mainOC->cadCreator.point1 = camera->cursorToXZPlane();
//			}
//			else
//			{
//				mainOC->renders[mainOC->currentRender]->addCircle
//				(mainOC->cadCreator.point1, magnitude3(camera->cursorToXZPlane() - mainOC->cadCreator.point1));
//
//				mainOC->cadCreator.isPoint1 = 0;
//				gv->cadMode = none;
//			}
//		}
//		if (gv->cadMode == sphere)
//		{
//			if (mainOC->cadCreator.isPoint1 == 0)
//			{
//				mainOC->cadCreator.point1 = camera->cursorToXZPlane();
//				mainOC->cadCreator.isPoint1 = 1;
//			}
//			else
//			{
//				mainOC->renders[mainOC->currentRender]->addSphereShape(mainOC->cadCreator.point1, magnitude3(camera->cursorToXZPlane() - mainOC->cadCreator.point1));
//				mainOC->cadCreator.isPoint1 = 0;
//				gv->cadMode = none;
//			}
//		}
//		if (gv->cadMode == extrusion)
//		{
//			mainOC->renders[mainOC->currentRender]->extrudeFace((gv->mPos.y - mainOC->cadCreator.point1.y) * 0.12);
//			mainOC->cadCreator.isExtruded = 0;
//			gv->cadMode = none;
//
//
//		}
//	}
//	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
//	{
//		gv->isLmbPressed = 0;
//	}
//
//	//RIGHT
//	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
//	{
//
//		if (gv->cadMode == polyline)
//		{
//			gv->cadPositions.back() = gv->cadPositions[0];
//			mainOC->cadCreator.lines.clear();
//			//print(mainOC->cadCreator.lines.positions.size()); 
//			//AUNQUE DIGA QUE LO ESTÁ LIMPIANDO NO ES EL CASO, ESTOY TENIENDO QUE BORRARLOS EN none
//			//std::cout <<"a "<< & mainOC->cadCreator.lines << "\n";
//			//PASO A MODO HARDCODED
//			mainOC->cadCreator.lines.addSet(gv->cadPositions);
//
//			mainOC->renders[mainOC->currentRender]->addPolyline(gv->cadPositions);
//			gv->cadPositions = { { 0,0,0 } };
//			gv->cadMode = none;
//			camera->setCursorToXZPoint(mainOC->renders[mainOC->currentRender]->positions[0]);
//
//
//		}
//	}
//
//
//	//MIDDLE
//	if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS)
//	{
//		gv->isMmbPressed = 1;
//		gv->LastMMPos = gv->mPos;
//	}
//	if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_RELEASE)
//	{
//		gv->isMmbPressed = 0;
//	}
//}
//
//
//float scrollTranslationSpeedFactor = 20;
//
//void scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
//	AllPointers* allPointers = static_cast<AllPointers*>(glfwGetWindowUserPointer(window));
//	Camera* camera = allPointers->camera;
//	GlobalVariables* gv = allPointers->gv;
//	Autopilot* autopilot = allPointers->autopilot;
//
//
//	if (yoffset > 0)
//	{
//		if (gv->program == MRS)
//		{
//			autopilot->world.totalXpixels *= 1.15;
//			autopilot->world.updateCamera();
//
//			//// --- keep current center fixed across zoom ---
//			//p2 screenCenter = { windowWidth * 0.5f,
//			//					windowHeight * 0.5f };
//			//p2 worldAtCenter = (screenCenter - autopilot->world.translationFactor) / autopilot->world.scalingFactor;
//
//			//autopilot->world.totalXpixels *= 1.15f;
//			//autopilot->world.updateCamera(); // recomputes scale & centers by design
//
//			//// restore so the same world point remains at screen center
//			//autopilot->world.translationFactor = screenCenter - worldAtCenter * autopilot->world.scalingFactor;
//		}
//		else if (gv->program == telemetry || gv->program == solarProgram || gv->program == openCascade || gv->program == offshoreProgram)
//		{
//			camera->cameraPos = camera->cameraPos + camera->forward * camera->translationSpeed * scrollTranslationSpeedFactor;
//		}
//	}
//	else if (yoffset < 0)
//	{
//		if (gv->program == MRS)
//		{
//			autopilot->world.totalXpixels /= 1.15;
//			autopilot->world.updateCamera();
//		}
//		else if (gv->program == telemetry || gv->program == solarProgram || gv->program == openCascade || gv->program == offshoreProgram)
//		{
//			camera->cameraPos = camera->cameraPos - camera->forward * camera->translationSpeed * scrollTranslationSpeedFactor;
//		}
//	}
//
//}

