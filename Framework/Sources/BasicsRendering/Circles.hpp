#pragma once

#include "Common.hpp"
#include "Lines2D.hpp"
#include "Polygons2D.hpp"


struct Circles {


	int r;
	int segments;

	float angleSegment; //theta increased by each segments

	Lines2D circunference;
	Polygons2D interior;

	Circles(int r_, int segments_ = 0) :r(r_) {

		/*if (segments_ != 0)
			segments = segments_;
		else
			segments = r_ * 3;*/

		if (segments_ == 0)
			segments = r_ * 3;
		else
			segments = segments_;


		angleSegment = 2 * PI / segments; //angle bewteern points in radians



	}


	void addSet(vector<p2> centerPositions) {

		//creates positions
		for (p2 center : centerPositions)
		{
			createCircle(center);
		}

	}

	void addSet(p2 centerPosition) {


		createCircle(centerPosition);


	}

	//creates positions and indices of a single circle for both objects
	void createCircle(const p2& center) {
		//interts the center, then each segment point and then the second point to close the triangles

		float theta;

		vector<p2> newPositions;
		newPositions.reserve(segments + 1);

		//starts in 0º,
		for (int i = 0; i < segments + 1; i++)
		{
			theta = (angleSegment * i);
			newPositions.push_back(p2{ center.x + r * cos(theta), center.y + r * sin(theta) });
		}


		circunference.addSet(newPositions);
		interior.addSet(newPositions);


	}

	void clear() {
		circunference.clear();
		interior.clear();
	}




	void drawInterior() {

		interior.draw();

	}
	void drawCircunference() {
		circunference.draw();

	}


};

//input in radians. It goes from angle1 to angle2, so angle2 is ensured to be ahead. 
// You can specify the number of segments or let it create it based on some arbitrary algorithm
vector<p2> createArc(p2 center, float r, float angle1, float angle2, int segments = 0) {

	vector<p2> positions;
	positions.reserve(segments + 1);

	while (angle1 > angle2)
	{
		angle2 += 2 * PI;
	}



	if (segments < 1)
	{
		segments = r * 3 * (angle2 - angle1) / (2 * PI);

		if (segments < 10) //it assumes that if you've put anything less than 10 you know why
			segments = 10;
	}

	float angleSegment = (angle2 - angle1) / segments; //angle bewteern points in radians


	for (int i = 0; i < segments + 1; i++)
	{
		float theta = angle1 + angleSegment * i;
		positions.push_back(p2{ center.x + r * cos(theta), center.y + r * sin(theta) });
	}


	return positions;
}

vector<p2> createRoundedSquare(p2 startingPos, p2 length, float radius) {
	vector<p2> positions;
	//reserve


	//bottom left
	vector<p2> temp = createArc(startingPos + radius, radius, radians(180), radians(270));
	positions.insert(positions.end(), temp.begin(), temp.end());

	temp = createArc({ startingPos.x + length.x - radius,startingPos.y + radius }, radius, radians(270), 0);
	positions.insert(positions.end(), temp.begin(), temp.end());

	temp = createArc({ startingPos.x + (length.x - radius),startingPos.y + (length.y - radius) }, radius, 0, radians(90));
	positions.insert(positions.end(), temp.begin(), temp.end());

	temp = createArc({ startingPos.x + radius,startingPos.y + length.y - radius }, radius, radians(90), radians(180));
	positions.insert(positions.end(), temp.begin(), temp.end());

	positions.emplace_back(positions.front());

	return positions;
}