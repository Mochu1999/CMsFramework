#pragma once

struct Timer
{
	string name;

	string output;


	//reference timestamp, relative 0
	std::chrono::high_resolution_clock::time_point startTime = std::chrono::high_resolution_clock::now();

	float accumulated = 0.0f; //all the time accumulated in the previous intervals
	float totalTime = 0.0f; //time 
	std::chrono::high_resolution_clock::time_point lastStartTime;

	bool running = 0;

	Timer(string name_)
		: name(name_)
	{
		lastStartTime = std::chrono::high_resolution_clock::now();
	}

	void stop()
	{
		if (!running) return;

		accumulated += std::chrono::duration<float>(std::chrono::high_resolution_clock::now() - lastStartTime).count();
		running = false;
	}

	void resume()
	{
		if (running) return;
		lastStartTime = std::chrono::high_resolution_clock::now();
		running = true;
	}

	void update()
	{

		if(running)
		{
			float currentInterval = std::chrono::duration<float>(std::chrono::high_resolution_clock::now() - lastStartTime).count();
			totalTime = currentInterval + accumulated;
		}
		else
		{
			totalTime = accumulated;
		}

		int total = static_cast<int>(totalTime);

		int h = total / 3600;
		int m = (total % 3600) / 60;
		int s = total % 60;

		std::ostringstream ss;
		ss /*<< name << ": "*/
			<< std::setw(2) << std::setfill('0') << h << ":"
			<< std::setw(2) << std::setfill('0') << m << ":"
			<< std::setw(2) << std::setfill('0') << s;
		//setw(2) sets a minimum field width of 2 characters. setfill sets those characters as 0 as default


		output = ss.str();

	}
};