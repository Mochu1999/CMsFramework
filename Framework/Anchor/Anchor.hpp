#pragma once

#include "Timer.hpp"

struct Anchor
{
	TimeStruct& tm;
	Shader& shaderText;

	Timer brainrot;
	Timer work;
	Timer pleasure;

	//Different texts to swap color
	Text textB;
	Text textW;
	Text textP;

	///////////////////////////////////////////////////////////////////////////
	//revisit text.hpp to look for the efficient method of updating the text...
	//also dpis
	//just show the last significant value and it's units
	//allow to stop and resume the timer


	Anchor (TimeStruct& tm_, Shader& shaderText_)
		:tm(tm_),shaderText(shaderText_)
		,brainrot("B"),work("W"),pleasure("P")
		, textB("resources/Glyphs/Helvetica/Helvetica.otf", 24), textW("resources/Glyphs/Helvetica/Helvetica.otf", 24)
		, textP("resources/Glyphs/Helvetica/Helvetica.otf", 24)

	{

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

		textB.addDynamicText({ {{ 10,windowWidth - 30 }, brainrot.output}, });
		textW.addDynamicText({ {{ 10,windowWidth - 60 }, work.output}, });
		textP.addDynamicText({ {{ 10,windowWidth - 90 }, pleasure.output}, });
	}

	void draw()
	{
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