#pragma once

#include "Common.hpp"
#include "Lines3D.hpp"
#include "AuxSquare.hpp"

struct Arrows {
	AuxSquare squares;
	Lines3D lines;
	vector<p3> positions;

	void addSet(vector<p3> firstPoints, vector<p3> normals) {
		vector<p3> secondPoints;
		secondPoints.reserve(firstPoints.size());

		for (int i = 0; i < firstPoints.size(); i++)
		{
			
			p3 secondPoint= firstPoints[i] + normals[i] * 10;
			positions.insert(positions.end(), { firstPoints[i],secondPoint });
			secondPoints.emplace_back(secondPoint);
		}
		squares.addSet(secondPoints);
		lines.addSet(positions, 2);
	}
	void draw() {
		squares.draw();
		lines.draw();
	}
	void clear()
	{
		lines.clear();
		positions.clear();
		// squares resets itself with addSet
	}
};

struct Arrows2
{
	Lines3D lines;
	vector<p3> positions;

	void addSet(vector<p3> firstPoints, vector<p3> normals, vector<float> magnitude)
	{
		lines.clear();
		positions.clear();

		vector<p3> secondPoints;
		secondPoints.reserve(firstPoints.size());

		for (int i = 0; i < firstPoints.size(); i++)
		{

			p3 secondPoint = firstPoints[i] + normals[i] * magnitude[i];
			positions.insert(positions.end(), { firstPoints[i],secondPoint });
			secondPoints.emplace_back(secondPoint);
		}

		lines.addSet(positions, 2);
	}
	void draw() {
		lines.draw();
	}
	
};