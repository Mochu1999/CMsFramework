#pragma once

#include "Timer.hpp"
#include "UI.hpp"
#include "FileManagementAnchor.hpp"
struct Anchor
{
	GLFWwindow* window; //just for UI drag, try to remove it
	GlobalVariables& gv;
	TimeStruct& tm;
	Shader& shader2D;
	Shader& shaderText;

	Timer brainrot;
	Timer work;
	Timer pleasure;
	UI ui;

	//Different texts to swap color
	Text textB;
	Text textW;
	Text textP;

	///////////////////////////////////////////////////////////////////////////
	//revisit text.hpp to look for the efficient method of updating the text...
	//also dpis
	//just show the last significant value and it's units
	//allow to stop and resume the timer

	
	//By doing this anchor has access to the close and minimize buttons, which isn't ideal, but whatever
	ButtonMode mode = None;
	string todaysBlock;

	Anchor(GLFWwindow* window_, GlobalVariables& gv_, TimeStruct& tm_, Shader& shader2D_, Shader& shaderText_)
		:window(window_), gv(gv_), tm(tm_), shader2D(shader2D_), shaderText(shaderText_)
		, ui(window, gv, shader2D_)
		, brainrot("B"), work("W"), pleasure("P")
		, textB("resources/Glyphs/Helvetica/Helvetica.otf", 24), textW("resources/Glyphs/Helvetica/Helvetica.otf", 24)
		, textP("resources/Glyphs/Helvetica/Helvetica.otf", 24)

	{
		checkDate();

		//This block is just to update the stopwatch in the case it wasn't closed properly, BUT IT SHOULD USE THE LAST SAVED TIME
		readTodaysBlock(todaysBlock);
		endStopwatch(todaysBlock);
		extractTodaysBlock();
		appendTodaysBlock(todaysBlock);

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//I don't know why it doesn't need readTodaysBlock, the structure of this is loose and needs to be rebuilt, sorry
		sync("1-", brainrot.accumulated);
		sync("2-", work.accumulated);
		sync("3-", pleasure.accumulated);
	}


	void logic()
	{
		switch (mode)
		{
		case B:
			if (!brainrot.running)
			{
				readTodaysBlock(todaysBlock);
				resumeStopwatch("1-", todaysBlock);
				extractTodaysBlock();
				appendTodaysBlock(todaysBlock);


				brainrot.resume();
				work.stop();
				pleasure.stop();

			}
			else
			{
				readTodaysBlock(todaysBlock);
				endStopwatch(todaysBlock);
				extractTodaysBlock();

				appendTodaysBlock(todaysBlock);
				brainrot.stop();
			}

			break;
		case W:
			if (!work.running)
			{
				readTodaysBlock(todaysBlock);
				resumeStopwatch("2-", todaysBlock);
				extractTodaysBlock();
				appendTodaysBlock(todaysBlock);

				brainrot.stop();
				work.resume();
				pleasure.stop();

			}
			else
			{
				readTodaysBlock(todaysBlock);
				endStopwatch(todaysBlock);
				extractTodaysBlock();

				appendTodaysBlock(todaysBlock);
				work.stop();
			}

			break;
		case P:
			if (!pleasure.running)
			{
				readTodaysBlock(todaysBlock);
				resumeStopwatch("3-", todaysBlock);
				extractTodaysBlock();
				appendTodaysBlock(todaysBlock);

				brainrot.stop();
				work.stop();
				pleasure.resume();

			}
			else
			{
				readTodaysBlock(todaysBlock);
				endStopwatch(todaysBlock);
				extractTodaysBlock();

				appendTodaysBlock(todaysBlock);
				pleasure.stop();
			}

			break;
		}
	}




	void update()
	{
		if (tm.counterUpdateAnchor > 0)
		{
			brainrot.update();
			work.update();
			pleasure.update();

			tm.counterUpdateAnchor = 0;
		}

		textB.addDynamicText({ {{ 10,windowHeight - 50 }, brainrot.output}, });
		textW.addDynamicText({ {{ 10,windowHeight - 80 }, work.output}, });
		textP.addDynamicText({ {{ 10,windowHeight - 110 }, pleasure.output}, });
	}

	void draw()
	{
		ui.draw();


		transparent();
		shaderText.bind();

		shaderText.setUniform("u_Color", 1.0f, 0.0f, 0.0f);
		textB.draw();

		shaderText.setUniform("u_Color", 0.0f, 1.0f, 0.3f);
		textW.draw();

		shaderText.setUniform("u_Color", 0.2f, 0.5f, 1.0f);
		textP.draw();
	}
};