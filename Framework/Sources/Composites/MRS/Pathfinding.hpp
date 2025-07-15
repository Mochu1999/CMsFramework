#pragma once


void calculateBoundingBox(vector<std::pair<p2, p2>>& boundingBoxes, const vector<vector<p2>>& lonLatPositions)
{
	boundingBoxes.clear();
	boundingBoxes.reserve(lonLatPositions.size());

	for (auto& vec : lonLatPositions)
	{
		float minX = std::numeric_limits<float>::max(), minY = std::numeric_limits<float>::max();
		float maxX = std::numeric_limits<float>::lowest(), maxY = std::numeric_limits<float>::lowest();

		for (auto& p : vec)
		{
			if (p.x < minX) minX = p.x;
			if (p.x > maxX) maxX = p.x;

			if (p.y < minY) minY = p.y;
			if (p.y > maxY) maxY = p.y;
		}

		boundingBoxes.push_back({ {minX,minY},{maxX,maxY} });
	}
}


//FUNCTION NEEDS TO BE HEAVILY TESTED. LOGIC OF COLLINEAR HAS NOT BEEN REVISED

//HOSTIAS, NECESITO QUE NO TOQUE EL PUNTO D NO CREARÁ 2 PUNTOS SI EN D CHOCAN DOS LINEAS

//Lines formed up by ab and cd 
//wettedSurface.hpp has a calculateIntersectionPoint that works similarly but between p3s, a triangle and a lines, these are 2 2d lines
bool isThereAnIntersectionPoint(const p2& a, const p2& b, const p2& c, const p2& d, p2& p)
{
	if (a == b || c == d) return false;

	//checking if they bounding boxes collide for an early exit
	if (std::max(a.x, b.x) < std::min(c.x, d.x) ||
		std::max(c.x, d.x) < std::min(a.x, b.x) ||
		std::max(a.y, b.y) < std::min(c.y, d.y) ||
		std::max(c.y, d.y) < std::min(a.y, b.y))
		return false; // bounding boxes do not overlap


	const float epsilon = 1e-6f;

	const p2 ab = { b.x - a.x ,b.y - a.y };
	const p2 cd = { d.x - c.x ,d.y - c.y };
	const p2 ac = { c.x - a.x ,c.y - a.y };

	/*
	In 2d geometry the cross product is a scalar value, not a vector, it represents the area of their parallelogram
	These are the possible cases:
	t = 0: Intersection at point a.
	0 < t < 1: Intersection between a and b (on the line segment).
	t = 1: Intersection at point b.
	t < 0: Intersection lies before a
	t > 1: Intersection lies beyond b
	*/


	const float precalculate = cross2(ab, cd);
	if (fabs(precalculate) < epsilon)
	{ // parallel/colinear branch

		if (fabs(cross2(ab, ac)) > epsilon)
			return false; // parallel but not colinear

		// Project c and d onto ab as scalars
		float t0 = dot2(c - a, ab);
		float t1 = dot2(d - a, ab);
		float denom = dot2(ab, ab);

		if (denom < epsilon) return false; // ab is too small – treat as no segment

		t0 /= denom;
		t1 /= denom;

		// Ensure t0 <= t1
		if (t0 > t1) std::swap(t0, t1);

		// Check overlap in [0, 1]
		if (t1 < 0 || t0 > 1)
			return false;

		// Return start of overlap (clamped to ab)
		float t = std::max(0.0f, t0);
		p = a + ab * t;
		return true;
	}

	float t = cross2(ac, cd) / precalculate; // (ac×cd)/(ab×cd)
	float u = cross2(ac, ab) / precalculate; // (ac×ab)/(ab×cd)

	if (t >= 0 && t <= 1 && u >= 0 && u < 1)
	{ // Return false for intersection in d
		p.x = a.x + t * ab.x;
		p.y = a.y + t * ab.y;
		return true;
	}

	return false;
}

//When routing finds a collision this struct stores the usefull data of the hit. In which polygon has it occurred, in what edge and the position
struct HitData
{
	unsigned int polygonIndex;
	unsigned int edge;
	p2 position;

	HitData(unsigned int polygonIndex_, unsigned int edge_, p2 position_) :polygonIndex(polygonIndex_), edge(edge_), position(position_)
	{}
	HitData() {}
};

//path will return the waypoints
//The algorithm finds a way from start to goal. it ends when path includes the goal
// It searches for a hit with a polygon in the path. If there are the path will go to the closest polygon and from there it will go to the vertex
// that is closest to the goal. The process is repeated from there
void routing(vector<p2>& path, const vector<vector<p2>> vecOfPolygons, p2 start, const p2 goal)
{
	path.push_back(start);

	while (path.back() != goal)
	{
		p2 closestPoint = goal;
		vector<HitData> hits;
		unsigned int hitIndex = std::numeric_limits<unsigned int>::max();

		//search for a hit with every polygon
		for (unsigned int i = 0; i < vecOfPolygons.size(); i++)
		{
			p2 currentHitPosition; //to be filled in isThereAnIntersectionPoint()

			for (unsigned int j = 0; j < vecOfPolygons[i].size(); j++)
			{
				if (isThereAnIntersectionPoint(path.back(), goal, vecOfPolygons[i][j], vecOfPolygons[i][j + 1], currentHitPosition))
				{
					if (path.back() != currentHitPosition) //Avoiding intersection with the point where you are
						hits.push_back({ i,j,currentHitPosition });
				}
			}
		}
		//searchs for the closest hit
		if (hits.size())
		{
			float lowestDistance = std::numeric_limits<float>::max();

			for (unsigned int i = 0; i < hits.size(); i++)
			{
				float distance = calculateDistance(start, hits[i].position);

				if (distance < lowestDistance)
				{
					lowestDistance = distance;
					closestPoint = hits[i].position;
					hitIndex = i;
				}
			}


		}
		//ends the while loop if you are in goal
		if (closestPoint == goal)
		{
			path.push_back(goal);
		}
		//we are in a polygon and we will be exiting it from the point that is closest to our goal
		else
		{

			const unsigned int polygonIndex = hits[hitIndex].polygonIndex;
			const unsigned int startEdgeIndex = hits[hitIndex].edge;
			const p2 position = hits[hitIndex].position;

			unsigned int endEdgeIndex = 0;
			float closestDistance = std::numeric_limits<float>::max();

			//we loop through the vertices looking for in which one will we be exiting
			for (size_t i = 0; i < vecOfPolygons[polygonIndex].size() - 1; i++) //the last point is repeated
			{
				float currentDistance = calculateDistance(vecOfPolygons[polygonIndex][i], goal);

				if (currentDistance < closestDistance)
				{
					closestDistance = currentDistance;
					endEdgeIndex = i;
				}
			}
			//Going from currentHit to closestIndex both ascending and descending to check where is
			const size_t length = vecOfPolygons[polygonIndex].size();

			vector<p2> ascendingPositions = { hits[hitIndex].position }, descendingPositions = { hits[hitIndex].position };
			float ascendingDistance = 0, descendingDistance = 0;

			for (size_t i = startEdgeIndex; i != endEdgeIndex; i = (i + 1) % length) //if i+i is over length is turns to 0
			{
				ascendingDistance += calculateDistance(ascendingPositions.back(), vecOfPolygons[polygonIndex][i]);
				ascendingPositions.push_back(vecOfPolygons[polygonIndex][i]);
			}


			//size_t is unsigned. If you are decreasing but not currently in 0 the +length will just, but if 0 it wraps to length-1
			for (size_t i = startEdgeIndex; i != endEdgeIndex; i = (i + length - 1) % length)
			{
				descendingDistance += calculateDistance(descendingPositions.back(), vecOfPolygons[polygonIndex][i]);
				descendingPositions.push_back(vecOfPolygons[polygonIndex][i]);
			}

			if (ascendingDistance < descendingDistance)
			{
				path.insert(path.end(), ascendingPositions.begin(), ascendingPositions.end());
			}
			else
			{
				path.insert(path.end(), descendingPositions.begin(), descendingPositions.end());
			}
		}

	}


}