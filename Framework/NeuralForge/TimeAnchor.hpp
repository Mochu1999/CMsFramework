#pragma once

#include <chrono>
using namespace std;
using namespace std::chrono;




struct TimeCounter {
	high_resolution_clock::time_point currentTime;
	high_resolution_clock::time_point lastTime;
	double endTime = std::numeric_limits<double>::max();
	TimeCounter() {
		currentTime = high_resolution_clock::now();
	}
	void endCounter() {
		lastTime = high_resolution_clock::now();
		endTime = duration_cast<duration<double>>(lastTime - currentTime).count();
		cout << "Elapsed time: " << endTime << "s" << endl;
	}


};



//if isRunning = 0 the counter keeps summing, if you want to stop it create a variable pausedTime that only adds time if paused and is always been rest from the sum

struct TimeStruct 
{
	//reference timestamp, relative 0
	std::chrono::high_resolution_clock::time_point startTime;
	//time since start: currentChronoTime - startTime
	float currentTime = 0.0f;
	//last frame's time, to use for deltaTime
	std::chrono::high_resolution_clock::time_point lastTime;

	float  deltaTime = 0.0f;


	//Anchor, just for rendering purposes of
	const float updateIntervalAnchor = 0.001f;
	float updateAccumulatorAnchor = 0;
	unsigned int counterUpdateAnchor = 0;

	TimeStruct() {
		lastTime = std::chrono::high_resolution_clock::now();
		startTime = lastTime;
	}

	void update()
	{
		auto currentChronoTime = std::chrono::high_resolution_clock::now();

		currentTime = std::chrono::duration<float>(currentChronoTime - startTime).count();

		deltaTime = std::chrono::duration_cast<std::chrono::duration<float>>(currentChronoTime - lastTime).count();
		lastTime = currentChronoTime;

		updateAnchor();
	}

	void updateAnchor()
	{
		updateAccumulatorAnchor += deltaTime;

		while (updateAccumulatorAnchor >= updateIntervalAnchor)
		{
			updateAccumulatorAnchor -= updateIntervalAnchor;
			++counterUpdateAnchor;
		}
	}
};


