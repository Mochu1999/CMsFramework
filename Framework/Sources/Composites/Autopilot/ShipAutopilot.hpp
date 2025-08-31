#pragma once

//Estoy pensando que Ship tenga las variables actuales del barco y course solo las cálculadas
//Idea: Que course sea efectivamente una clase sin apenas variables y que las que necesite de ship las reciva por función (y no construcción)

struct Course
{

};

struct ShipAutopilot
{
	TimeStruct& tm;

	Course course;

	//Si esto está aquí traete circleFinish de vuelta, tiene sentido que esté data en ui porque es general, circles no
	Lines2D courseLine;
	vector<p2> pathWaypoints;


	p2 shipCoordinates = { 2.128842,41.248926 };
	p2 finishPoint = { 1.25,39.05 };

	float speed = 3; //m/s
	float dt = 3600.0f; // s

	float angle = 0;

	float totalDistance = 0;


	float& scalingFactor;
	p2& translationFactor;

	ShipAutopilot(TimeStruct& tm_, float& scalingFactor_, p2& translationFactor_)
		:tm(tm_)
		, scalingFactor(scalingFactor_), translationFactor(translationFactor_)
	{

		pathWaypoints.push_back(shipCoordinates);
		vector<p2> intermediateWaypoints = divideGreatCircle(shipCoordinates, finishPoint, 5000);
		pathWaypoints.insert(pathWaypoints.end(), intermediateWaypoints.begin(), intermediateWaypoints.end());
		pathWaypoints.push_back(finishPoint);

		//print(pathWaypoints);

		courseLine.clear();
		courseLine.addSet(lonLatToMercator(pathWaypoints));
	}

	void updateShipPosition()
	{

		/* //Random noise generator to end in a different place than what was simulated
		std::default_random_engine rng(std::random_device{}());
		std::uniform_real_distribution<float> dist(-0.03f, 0.03f);

		// Apply noise directly to all but the last waypoint
		for (size_t i = 0; i + 1 < currentWaypoints.size(); ++i) {
			currentWaypoints[i].x += dist(rng);
			currentWaypoints[i].y += dist(rng);
		}
		*/
		//distance recorring every waypoint

		totalDistance = 0.0f;
		p2 prev = shipCoordinates;
		for (const auto& wp : pathWaypoints) {
			totalDistance += calculateDistance(prev, wp);
			prev = wp;
		}

		//if (!pathWaypoints.empty())
		//{
		//	float distToNext = calculateDistance(pathWaypoints[0], pathWaypoints[1]);

		//}
		//else
		//{
		//	speed = 0;
		//}

		courseLine.clear();
		courseLine.addSet(lonLatToMercator(pathWaypoints));

		//calculate angle
		if (!pathWaypoints.empty())
		{
			//where is the ship pointing at
			p2 interm = pathWaypoints[1] - pathWaypoints[0];
			angle = degrees(atan2(interm.y, interm.x));
		}
		else angle = 0;
	}


	void draw()
	{

	}
};