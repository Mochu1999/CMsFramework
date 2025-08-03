#pragma once
#include "Icons.hpp"


//Sin Implementar
struct ShipMRS
{
	//////////inputs
	p2 lonLatPosition;
	float globalCourse;
	float driftAngle;
	//Custom realTimeRepeatedMeteorology

	float windSpeed;
	p2 windDirection;

	struct AIS
	{
		p2 position;
		float speed;
		float course;

	};
	vector<AIS> aisShips;
	float batteryLevel;


	//////////outputs
	string telemEtryMRS;
	unsigned int pulsesRudder;
	unsigned int pulsesFlap1;
	unsigned int pulsesFlap2;
	unsigned int pulsesFlap3; //direction

};




struct shipMRSData
{
	Shader& shader2D;
	Camera& camera;
	TimeStruct& tm;

	IconLight icon;

	Lines2D courseLine;
	vector<p2> pathPositions;
	vector<p2> currentWaypoints;

	Circles circle;

	p2 shipCoordinates = { 2.128842,41.248926 };
	p2 finishPoint = { 1.25,39.05 };
	p2 vect;

	float speed = 3; //m/s
	float dt = 3600.0f; // s

	float totalDistance = 0;
	
	matrix4x4 mapModel2DMatrix;

	shipMRSData(Shader& shader2D_, Camera& camera_, TimeStruct& tm_)
		:shader2D(shader2D_), camera(camera_),tm(tm_), icon(shader2D, camera), circle(5000, 100)
	{
		circle.addSet(lonLatToMercator({ finishPoint }));


		currentWaypoints = { {1.754639,41.05185},{1.383457,40.78217},{1.232,40.474},{ 0.8985715,40.01441},
		{  0.7449858 ,39.48259},{0.9035207,39.20287},finishPoint };

		pathPositions.push_back(shipCoordinates);
		pathPositions.insert(pathPositions.end(), currentWaypoints.begin(), currentWaypoints.end());
		pathPositions.push_back(finishPoint);

		//calculateDistances here, before transforming positions into mercator

		pathPositions = lonLatToMercator(pathPositions);

		courseLine.clear();
		courseLine.addSet(pathPositions);
	}
	
	void updateShipPosition() 
	{
		float deltaDistance = speed * 3600.0f;
		totalDistance = 0.0f;

		std::default_random_engine rng(std::random_device{}());
		std::uniform_real_distribution<float> dist(-0.03f, 0.03f);

		// Apply noise directly to all but the last waypoint
		for (size_t i = 0; i + 1 < currentWaypoints.size(); ++i) {
			currentWaypoints[i].x += dist(rng);
			currentWaypoints[i].y += dist(rng);
		}

		p2 prev = shipCoordinates;
		for (const auto& wp : currentWaypoints) {
			totalDistance += calculateDistance(prev, wp);
			prev = wp;
		}
		
		
		if (!currentWaypoints.empty()) 
		{
			float distToNext = calculateDistance(shipCoordinates, currentWaypoints.front());

			

			/*print(deltaDistance);
			print(distToNext);*/

			if (deltaDistance >= distToNext) 
			{
				// Avanza completamente al siguiente waypoint
				deltaDistance -= distToNext;
				shipCoordinates = currentWaypoints.front();
				currentWaypoints.erase(currentWaypoints.begin());
			}
			else 
			{
				p2 currentObjective = currentWaypoints.front();

				float t = deltaDistance / distToNext;
				shipCoordinates = {
					shipCoordinates.x + (currentObjective.x - shipCoordinates.x) * t,
					shipCoordinates.y + (currentObjective.y - shipCoordinates.y) * t
				};
			}
		}
		else
		{
			speed = 0;
		}

		// reconstruir la ruta visual
		pathPositions.clear();
		pathPositions.push_back(shipCoordinates);
		pathPositions.insert(pathPositions.end(), currentWaypoints.begin(), currentWaypoints.end());
		pathPositions.push_back(finishPoint);

		std::vector<p2> mercatorPath = lonLatToMercator(pathPositions);
		courseLine.clear();
		courseLine.addSet(mercatorPath);
	}


	void draw(p2 a)
	{

		shader2D.bind();

		//surprisingly it won't work without the lonlat. I don't know why. Point error? Projection?
		
		if (!currentWaypoints.empty())
			vect = lonLatToMercator( currentWaypoints.front() - shipCoordinates);
		float angle = degrees(atan2(vect.y, vect.x));


		

		shader2D.setUniform("u_Color", 1, 0.3, 0, 0.5);
		glLineWidth(1);
		courseLine.draw();
		glLineWidth(1);





		shader2D.setUniform("u_Color", 1, 0, 0, 1);
		circle.drawInterior();



		//Icon can't use the map model matrix, it would need have its positions at 2e+06 for it
		mapModel2DMatrix = camera.create2DModelMatrix(a, angle, 50);
		shader2D.setUniform("u_Model", mapModel2DMatrix);
		icon.draw();
	}
};