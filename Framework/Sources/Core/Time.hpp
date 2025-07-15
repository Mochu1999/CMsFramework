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

struct TimeStruct {
	std::chrono::high_resolution_clock::time_point lastFrameTime;
	std::chrono::high_resolution_clock::time_point startElapsedTime;

	float currentTime = 0.0f;

	float fps = 0.0f;
	float frameCount = 0.0f;
	float timeAccumulator = 0.0f;
	
	float plotTimeAccumulator = 0.0f;
	const float plotUpdateInterval = 0.0167f;
	float fiveSecondsAccumulator = 0; //to update auxVerticalGridLines from the plot
	unsigned int counterSecondsPlot = 0;
	unsigned int counterUpdatePlot = 0;

	//MRS
	const float mRSUpdateInterval = 1;
	float mRSTimeAccumulator = 0;
	unsigned int counterUpdateMRS = 0;

	TimeStruct() {
		lastFrameTime = std::chrono::high_resolution_clock::now();
		startElapsedTime = lastFrameTime;
	}

	void update() 
	{
		auto currentFrameTime = std::chrono::high_resolution_clock::now();

		currentTime = std::chrono::duration<float>(currentFrameTime - startElapsedTime).count();

		float  deltaTime = std::chrono::duration_cast<std::chrono::duration<float>>(currentFrameTime - lastFrameTime).count();
		lastFrameTime = currentFrameTime;

		frameCount++;
		timeAccumulator += deltaTime;
		plotTimeAccumulator += deltaTime;
		fiveSecondsAccumulator += deltaTime;

		if (currentTime>2)
			mRSTimeAccumulator += deltaTime;

		if (timeAccumulator >= 0.5f) 
		{
			fps = frameCount / timeAccumulator;
			frameCount = 0;
			timeAccumulator -= 0.5f;
		}

		updatePlot();
		updateMRS();
	}

	void updatePlot() 
	{
		
		if (plotTimeAccumulator >= plotUpdateInterval) 
		{
			plotTimeAccumulator -= plotUpdateInterval; // Reset accumulator

			if (fiveSecondsAccumulator >= 1)
			{
				fiveSecondsAccumulator -= 1;
				++counterSecondsPlot;
			}

			counterUpdatePlot++;
		}
	}
	void updateMRS()
	{
		if (mRSTimeAccumulator >= mRSUpdateInterval)
		{
			mRSTimeAccumulator -= mRSUpdateInterval;

			counterUpdateMRS++;
		}
	}
};


