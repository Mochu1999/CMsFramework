#pragma once

//all controls for Anchor
enum class ButtonMode {
	None, //none
	B,
	W,
	P,
	Close,
	Minimize,
	Drag
};
using enum ButtonMode;

//stores a hitbox, a polygon and a ButtonMode
struct Button
{
	ButtonMode id = None;
	vector<p2> positions;
	Polygons2D square;

	// a default constructor so it doesn't need to be constructer in the 
	// initializer list, but assigned isInsideHitBox another constructor
	//not using the initializer list is bad for performance
	//Button() {} 
	Button(ButtonMode id_, p2 corner1, p2 corner2)
		:id(id_)
	{
		square.addSet(corner1, corner2);
		positions = square.positions;
	}
	/*Button(vector<p2> items)
	{
		square.addSet(items);
	}*/

	void draw()
	{

		square.draw();
	}
};




struct UI
{
	GLFWwindow* window;
	GlobalVariables& gv;
	Shader& shader2D;

	//all buttons
	Button buttonClose;
	Button buttonMinimize;
	Button buttonDrag;

	Button buttonB;
	Button buttonW;
	Button buttonP;

	ButtonMode hoveredId = None;
	ButtonMode pressedId = None;

	Polygons2D dragBar;
	bool isDraggingWindow=false;
	double dragStartCursorX = 0.0;
	double dragStartCursorY = 0.0;
	int dragStartWinX = 0;
	int dragStartWinY = 0;

	UI(GLFWwindow* window_,GlobalVariables& gv_, Shader& shader2D_)
		:window(window_), gv(gv_), shader2D(shader2D_)
		, buttonClose(Close, p2{ windowWidth - 20 * sqrt2, windowHeight - 20 }, p2{ windowWidth, windowHeight })
		, buttonMinimize(Minimize, p2{ windowWidth-40*sqrt2, windowHeight-20 }, p2{ windowWidth-20*sqrt2, windowHeight})
		, buttonDrag(Drag, p2{ 0, windowHeight - 20}, p2{windowWidth, windowHeight})
		, buttonB(B, p2{ 110, 70 }, p2{ 140, 90 })
		, buttonW(W, p2{ 110, 40 }, p2{ 140, 60 })
		, buttonP(P, p2{ 110, 10 }, p2{ 140, 30 })
	{
		dragBar.addSet({0,windowHeight-20}, { windowWidth,windowHeight });
	}

	static bool isInsideHitBox(const Button& b, const p2& m)
	{
		return (m.x > b.positions[0].x && m.x < b.positions[1].x &&
			m.y > b.positions[0].y && m.y < b.positions[2].y);
	}
	//to know where I am if mouse button get's pressed
	ButtonMode checkHitBoxes(const p2& m) const
	{
		if (isInsideHitBox(buttonB, m)) return buttonB.id;
		else if (isInsideHitBox(buttonW, m)) return buttonW.id;
		else if (isInsideHitBox(buttonP, m)) return buttonP.id;
		else if (isInsideHitBox(buttonClose, m)) return buttonClose.id;
		else if (isInsideHitBox(buttonMinimize, m)) return buttonMinimize.id;
		else if (isInsideHitBox(buttonDrag, m)) return buttonDrag.id; //if drag is after close and minimize, they won't interact

		else return None;
	}


	ButtonMode processMouseButton(int glfwButton, int action)
	{
		//the pressed button must be the left one
		if (glfwButton != GLFW_MOUSE_BUTTON_LEFT) return None;

		//action gets triggered on release, pressedId allows to see the current state and drag out the hitbox to cancel
		if (action == GLFW_PRESS)
		{
			pressedId = checkHitBoxes(gv.mPos);

			if (pressedId == Drag)
			{
				isDraggingWindow = true;
				glfwGetCursorPos(window, &dragStartCursorX, &dragStartCursorY);
			}

			return None;
		}

		

		if (action == GLFW_RELEASE)
		{
			if (isDraggingWindow)
				isDraggingWindow = false;

			//checks if you are in the same state of pressedId, it does nothing and cancels pressedId otherwise
			ButtonMode releaseId = checkHitBoxes(gv.mPos);

			if (pressedId != None && releaseId == pressedId)
			{
				pressedId = None;
				return releaseId;
			}
			else
			{
				pressedId = None;
				return None;
			}
		}
		return None; //In case the action is none of the above, which shouldn't happen, but whatever
	}

	void updateWindowDrag()
	{
		if (!isDraggingWindow) return;

		double cx, cy;
		glfwGetCursorPos(window, &cx, &cy);

		int winX, winY;
		glfwGetWindowPos(window, &winX, &winY);

		// cursor in screen coords
		int screenCursorX = winX + (int)cx;
		int screenCursorY = winY + (int)cy;

		// move window so the cursor stays at the same grab offset inside the window
		int newX = screenCursorX - (int)dragStartCursorX;
		int newY = screenCursorY - (int)dragStartCursorY;

		glfwSetWindowPos(window, newX, newY);
	}

	void drawButton(Button& b)
	{
		if (pressedId == b.id)
			shader2D.setUniform("u_Color", 0.35f, 0.35f, 0.35f, 1.0f);
		else if (hoveredId == b.id)
			shader2D.setUniform("u_Color", 0.219f, 0.219f, 0.219f, 1);
		else
			shader2D.setUniform("u_Color", 0.121f, 0.121f, 0.121f, 1);

		b.draw();
	}

	void draw()
	{
		updateWindowDrag();
		hoveredId = checkHitBoxes(gv.mPos);
		//if it's hovering, it will already change it's color


		//opaque();
		transparent();
		shader2D.bind();
		shader2D.setUniform("u_Model", identityMatrix);

		shader2D.setUniform("u_Color", 0.35f, 0.35f, 0.35f, 1.0f);
		dragBar.draw();



		drawButton(buttonClose);
		drawButton(buttonMinimize);

		drawButton(buttonB);
		drawButton(buttonW);
		drawButton(buttonP);
	}

};
