#pragma once
#include "AuxAutopilot.hpp"
#include "SweepTriangulation.hpp"

std::vector<unsigned int> triangulateFromP2(const std::vector<p2>& polygon)
{
	// Convert p2 -> p3 with z=0
	std::vector<p3> verts;
	verts.reserve(polygon.size());
	for (auto& v : polygon)
		verts.push_back({ v.x, v.y, 0.0f });

	// Call your sweep triangulation
	SweepTrAux tri;
	return tri.sweepTriangulation(verts);
}


struct World
{
	vector<vector<p2>> allLonLats; //all polylines in lonLats
	vector<vector<p2>> safeZones;
	Lines2D mercator, mercatorSafe; //proyected map, only for visualization
	Lines2D frame; //outer rectangle of mercator

	Polygons2D background;
	vector<Polygons2D> polygons;

	//some reference variables used for visualization
	p2 point0; //bottom left corner
	float totalXmeters, totalYmeters; //horizontal and vertical distances
	float totalXpixels = 1000 * 6; //This makes totalXmeters 6000 pixels long //Stored in settings


	//these variables are the scale and translation of the Model Matrix
	float scalingFactor;
	p2 mapCorner, translationFactor;




	World()
	{
		//allLonLats NO SE ESTÁ USANDO, SI NO LO VAS A USAR ELIMINALO
		readMapBinary("map2.txt", allLonLats, mercator, frame);
		//writeMapBinary(allLonLats, "map2.txt");

		safeZones = createSafeZone(allLonLats, 0.05);

		//Starting with the first point after the frame
		for (size_t i = 1; i < safeZones.size(); i++)
		{
			vector<p2>interm = lonLatToMercator(safeZones[i]);
			mercatorSafe.addSet(interm);
		}

		//Last try trying to visualize polygons infill
		for (size_t i = 1; i < allLonLats.size(); i++)
		{
			Polygons2D interm;
			std::vector<unsigned int> indices = triangulateFromP2(allLonLats[i]);
			interm.addSet(lonLatToMercator(allLonLats[i]), indices);
			polygons.push_back(interm);
		}


		background.addSet(frame.positions);

		point0 = frame.positions[0];
		totalXmeters = frame.positions[1].x - frame.positions[0].x;
		totalYmeters = frame.positions[2].y - frame.positions[1].y;


		//Multiplying the map positions by scalingFactor in the model matrix will make all coordinates fall between 0 and totalXpixels (6000 pixels)
		scalingFactor = totalXpixels / totalXmeters;

		//initial translating factor
		//after scalingFactor, point0 falls somewhere {-500.242,652.636}. We will sum that amount to put point0 in O
		translationFactor = -point0 * scalingFactor;
		//point0 is at the bottom left of the screen, now we move it arbitrarily some amount so it's centered
		translationFactor += windowCenter - (totalXpixels * 0.5f);
		updateCamera();
	}

	/*
		Pipeline:		screen = scalingFactor * world + translationFactor
		world: coordinates in meters(Mercator space).
		scalingFactor: how many pixels per meter.
		translationFactor: how much we shift everything in pixels
		*/
	//We have maps of over 10^6 meters, and we want them (the totalXmeters, arbitrarily) to be of a certain size (totalXpixels)
	void updateCamera()
	{

		//world = (screen - translationFactor) / scalingFactor
		p2 worldAtCenter = (windowCenter - translationFactor) / scalingFactor;


		//Scrolling the mouse's wheel changes totalXpixels
		scalingFactor = totalXpixels / totalXmeters;

		translationFactor = windowCenter - worldAtCenter * scalingFactor;

		
	}


	//Y POR QUE ESTO NO VA A AUX?
	//CONVERSOR A METROS
	// Esto no es preciso, debería de hacerse Per-vertex local tangent plane
	// Bisector may fail, not limit tested
	//offsetted polygon around input, each point of it falls in the bisector angle
	vector<vector<p2>> createSafeZone(vector<vector<p2>>& input, float distance)
	{
		vector<vector<p2>> output;
		int count = 0;
		for (auto& polygon : input)
		{
			int size = polygon.size() - 1;

			vector<p2> interm;

			//Creates a point each iteration of the loop
			for (int i = 0; i < size; i++)
			{
				int prevIndex, nextIndex;

				//method that wraps around first and last point
				prevIndex = (i - 1 + size) % size;
				nextIndex = (i + 1) % size;

				//These 3 points are needed to create each point
				p2 current = polygon[i];
				p2 previous = polygon[prevIndex];
				p2 next = polygon[nextIndex];

				// vectors Previous-Current and Current-Next respectively
				p2 PC = normalize2(previous - current);
				p2 CN = normalize2(next - current);

				p2 bisector = normalize2(PC + CN);

				//bisector will be facing inside if that cross product is smaller than 0
				if (cross2(PC, CN) < 0.0f) bisector = -bisector;

				interm.push_back(current + bisector * distance);
				removeInnerLoops(interm);
			}
			interm.push_back(interm[0]);//first point repeated
			//print(count);
			//print(interm);

			output.push_back(interm);

			//if (count == 10) break;
			count++;
		}
		return output;
	}

	//checking each time a point is created
	void removeInnerLoops(vector<p2>& polygon)
	{
		p2 point; //not needed here

		if (polygon.size() < 2)return;
		size_t j = 0;
		if (polygon.size() > 20) j = polygon.size() - 20;

		for (; j < polygon.size() - 2; j++)
		{
			if (calculateIntersectionPoints(polygon[polygon.size() - 2], polygon[polygon.size() - 1], polygon[j], polygon[j + 1], point))
			{

				polygon.erase(polygon.begin() + j, polygon.begin() + polygon.size() - 1);

				return;
			}
		}
	}


};
