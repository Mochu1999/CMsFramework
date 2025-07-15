#pragma once

#include "Common.hpp"


#include "Polygons.hpp"
#include "Fourier.hpp"

struct WettedSurface
{
	struct Intersections {
		p3 point;
		unsigned int segment;//to what vertex it corresponds
		unsigned int imm; //immediate
		Intersections() : point({ 0,0,0 }), segment(0), imm(0) {} //initializing paramenters to 0 if not specified 

		Intersections(p3 point_, unsigned int segment_, unsigned int imm_)
			: point(point_), segment(segment_), imm(imm_) {}
	};


	vector<Intersections> intersections;

	Fourier& fourier; //SOBRA?
	vector<p3>& fourierPositions = fourier.positions;
	vector<unsigned int>& fourierIndices = fourier.indices;


	vector<vector<unsigned int>> validIndices;


	vector<unsigned int> batchIndices;

	Polygons& inputPolygon;
	vector<p3>& polygonPositions = inputPolygon.lines.positions;

	vector<p3> positions;



	WettedSurface(Polygons& inputPolygon_, Fourier& fourier_)
		: inputPolygon(inputPolygon_), fourier(fourier_)
	{
		
	}
	//NO TENIAS UNA QUE NO USABA O(N^2)
	//Finds all intersections and fills them  with intersections, segments and immediates 
	void calculateIntersections() {

		intersections.clear();

		// j = current polygon index
		for (unsigned int j = 0; j < polygonPositions.size() - 1; j++)
		{
			for (int i = 0; i < fourierIndices.size(); i += 3)
			{
				p3 point;
				//cout << endl << endl << endl;
				//cout << "triangle formed by: " << fourierIndices[i] << " " << fourierIndices[i + 1] << " " << fourierIndices[i + 2] << endl;

				p3 a = fourierPositions[fourierIndices[i]];
				p3 b = fourierPositions[fourierIndices[i+1]];
				p3 c = fourierPositions[fourierIndices[i+2]];

				if (calculateIntersectionPoint(a, b, c, polygonPositions[j], polygonPositions[j + 1], point))
				{
					if (intersections.size()&&point == intersections.back().point) break;
					//we do not add it directly into intersections because it may be an invalid point (when crossProduct == 0)
					Intersections possibleIntersection = { point,j,j };

					float crossProductJ = isBelowTriangle(a,b,c, polygonPositions[j]);


					//If crossProductJ > 0, j is above the water, then the immediate is j++, else j is the immediate
					if (crossProductJ > 0)
					{
						possibleIntersection.imm++;
						intersections.emplace_back(std::move(possibleIntersection));
					}
					else if (crossProductJ < 0)
					{
						intersections.emplace_back(std::move(possibleIntersection));
					}


					//when the point is on a index, it takes the next and previous indices to decide if there is an intersection
					// It doesn't detect an intersection if both immediates are over or under the point. So one point must be under 
					// and the other above (or at 0) to be an intersection. This way we are forcing an even number of intersections
					else if (crossProductJ == 0)
					{
						int prevIndex, nextIndex;

						unsigned int size = polygonPositions.size() - 1;
						prevIndex = (j - 1 + size) % size;
						nextIndex = (j + 1) % size;


						float crossProductPrevIndex = isBelowTriangle(a,b,c, polygonPositions[prevIndex]);

						float crossProductNextIndex = isBelowTriangle(a,b,c, polygonPositions[nextIndex]);

						if (crossProductNextIndex < 0 && crossProductPrevIndex >= 0)
						{

							possibleIntersection.imm = nextIndex;
							intersections.emplace_back(std::move(possibleIntersection));

						}

						else if (crossProductNextIndex >= 0 && crossProductPrevIndex < 0)
						{

							possibleIntersection.imm = prevIndex;
							intersections.emplace_back(std::move(possibleIntersection));
						}



					}
					//if none it won't add anything to intersections


				}

			}
		}
		
		p3 left = cross3(inputPolygon.normal, p3{0,-1,0});

		std::sort(intersections.begin(), intersections.end()
			, [&left](const Intersections& a, const Intersections& b)
			{
				// Project points a and b onto the 'left' vector
				float projA = dot3(a.point, left);
				float projB = dot3(b.point, left);
				return projA < projB;  // Sort from 'left' to 'right'
			});


		

		/*cout << "intersections " << endl;
		for (const auto& item : intersections)
		{
			cout << "{{" << item.point.x << "," << item.point.y << "," << item.point.z << "},"
				<< item.segment << "," << item.imm << "}" << endl;
		}cout << endl;*/

		



		
	}

	void draw() {

		//inputPolygon.translate({ 0,0.1,0 });
		//printv3(inputPolygon.positions);

		inputPolygon.draw();

		calculatePositions();

		//printv3(positions);

	}


	//In sweepTriangulation I use structuredPoints, here I use iterators and distances. They are doing the same

//It links interesection points in pairs, jumping from a first one into a second one till all surfaces are closed
	void calculatePositions() {
		//cout << "inputPolygon.positions.size() " << inputPolygon.positions.size() << endl;

		calculateIntersections();


		batchIndices.clear();
		positions.clear();


		//If there are intersections, it creates the positions of the wetted surface
		if (intersections.size())
		{
			//stores distances. Distances here are equivalent to the intersection indices 
			std::unordered_set<int> usedDistances;
			usedDistances.reserve(intersections.size());

			//vector<unsigned int> debugInfo; //to store the info for debug purposes

			//I'll be using iterator instead of direct indexing for no particular reason
			std::vector<Intersections>::iterator initialIt, firstIt, secondIt;
			unsigned int distanceInitialIt, distanceFirstIt, distanceSecondIt;



			batchIndices.emplace_back(0);

			//outer loop, stays here until all surfaces are closed
			while (usedDistances.size() < intersections.size())
			{


				distanceInitialIt = 0;


				//looks for distanceInitialIt, which it is the first non used distance of the intersections
				while (usedDistances.find(distanceInitialIt) != usedDistances.end())
				{
					++distanceInitialIt; //increments till you find one that is not in the set
				}


				initialIt = intersections.begin() + distanceInitialIt;

				firstIt = initialIt;

				distanceFirstIt = distanceInitialIt;



				positions.emplace_back((*firstIt).point);

				//Premise: no distance in a surface will be higher than that of the first secondImm


				int distanceCheck = -1;// invalid distance to enter the while loop

				int whileCounter = 0;



				//Stays here till the surface closes. You get here with firstIt already known. Calculates secondIt, goes from first to second
				//  with getImmediates if applies,and second to the next first with getWavePoints
				while (distanceCheck != 0)
				{
					//cout << "whileCounter " << whileCounter << endl;
					if (whileCounter > 20)
					{
						//isRunning = false; //FALLO AL CREAR GlobalVariables
						cout << "aborting wetted surface: high whileCounter" << endl;
						return;
						//std::exit(EXIT_FAILURE);
					}
					whileCounter++;


					

					//finding secondIt
					if (firstIt == initialIt)
						distanceSecondIt = calculateDistanceSecondIt(1, firstIt, distanceInitialIt, distanceFirstIt);

					else
						distanceSecondIt = calculateDistanceSecondIt(0, firstIt, distanceInitialIt, distanceFirstIt);

					//cout << "distanceFirstIt:" << distanceFirstIt << ", distanceSecondIt: " << distanceSecondIt << endl;

					secondIt = intersections.begin() + distanceSecondIt;



					bool areImmediates = 1; //activates getImmediates
					if ((*firstIt).segment == (*secondIt).segment)
						areImmediates = 0;


					if (areImmediates) {
						getImmediates((*firstIt).imm, (*secondIt).imm);
					}
					positions.emplace_back((*secondIt).point);



					usedDistances.emplace(distanceFirstIt);
					usedDistances.emplace(distanceSecondIt);


					//The key in all of this. secondIt is linked to its pair, to be used in the next iteration
					if (distanceSecondIt % 2 == 0) {
						distanceFirstIt = distanceSecondIt + 1;
					}
					else {
						distanceFirstIt = distanceSecondIt - 1;
					}



					//for the next iteration
					firstIt = intersections.begin() + distanceFirstIt;


					//getWavePoints((*firstIt).point.x, (*secondIt).point.x);


					positions.emplace_back((*firstIt).point);

					distanceCheck = distanceInitialIt - distanceFirstIt; //if 0 you have arrived to initialIt again and the loop ends

				}

				batchIndices.emplace_back(positions.size());
			}


		}
		//else //no intersections, we will check if a random index is under or over the water
		//{
		//	int firstIndex = static_cast<int>((polygonPositions[0].x - fourierPositions[0].x) * fourier.intervalInverse);

		//	if (isRightOfLine(fourierPositions[firstIndex], fourierPositions[firstIndex + 1], polygonPositions[0]) < 0)
		//	{
		//		positions.reserve(polygonPositions.size());
		//		positions = polygonPositions;
		//		batchIndices.insert(batchIndices.end(), { 0,static_cast<unsigned int>(polygonPositions.size()) });
		//	}




		//	//float centroidX = inputPolygon.centroid[0] * 1000;
		//	//float centroidY = inputPolygon.centroid[1] * 1000;

		//	//int firstIndex = (centroidX - fourierPositions[0]) / fourier.interval * 2;


		//	//// Ensure even indices
		//	//firstIndex -= firstIndex % 2;

		//	//int secondIndex = firstIndex + 2;

		//	//if (isRightOfLine(fourierPositions[firstIndex], fourierPositions[firstIndex + 1]
		//	//	, fourierPositions[secondIndex], fourierPositions[secondIndex + 1], centroidX, centroidY) < 0)
		//	//{
		//	//	positions.reserve(polygonPositions.size());
		//	//	positions = polygonPositions;
		//	//}

		//}

	}


	//Looks for the segment that closer to Initial. If two intersections share same segment it takes the one that is closer to its index
	unsigned int calculateDistanceSecondIt(bool isInitial, auto& firstIt
		, unsigned int& distanceInitialIt, unsigned int& distanceFirstIt) {

		unsigned int distanceSecondIt; //output

		unsigned int currentMinSegment = std::numeric_limits<unsigned int>::max();
		unsigned int correctedSegment; float savedX;

		int start, end, step;
		if (isInitial) {
			start = distanceInitialIt + 1;
			end = intersections.size();
			step = 1;
		}
		else {
			start = distanceFirstIt - 1;
			end = distanceInitialIt;
			step = -1;
		}


		for (int j = start; isInitial ? j < end : j > end; j += step)
		{
			//(*firstIt).segment and those bigger than it stay the same but those that are smaller are increased by positions.size()-1
			// so that everything is bigger than (*initialIt).segment). Those that were behind initialIt now are correctly the farther from it
			if (intersections[j].segment < (*firstIt).segment)
				correctedSegment = intersections[j].segment + polygonPositions.size() - 1;
			else
				correctedSegment = intersections[j].segment;

			bool continuing = true;

			//if j is in the same segment as initial it can't have less distance to it's vertex than initial
			if ((*firstIt).segment == correctedSegment)
			{
				if ((abs(intersections[j].point.x - polygonPositions[intersections[j].segment].x)
					- abs((*firstIt).point.x - polygonPositions[intersections[j].segment].x)) < 0)
				{
					continuing = false;
					//cout << "se quita de la pull j = " << j << endl;
				}
			}
			if (continuing)
			{
				if (correctedSegment < currentMinSegment)
				{
					currentMinSegment = correctedSegment;

					savedX = abs(intersections[j].point.x - polygonPositions[intersections[j].segment].x); //for the third case

					distanceSecondIt = j;
				}
				else if (correctedSegment == currentMinSegment)
				{

					if (abs(intersections[j].point.x - polygonPositions[intersections[j].segment].x) < savedX)
					{
						currentMinSegment = correctedSegment;

						savedX = abs(intersections[j].point.x - polygonPositions[intersections[j].segment].x);

						distanceSecondIt = j;
					}
				}
				//cout << "j: " << j << ", intersections[j].segment: " << intersections[j].segment << ", correctedSegment: " << correctedSegment << endl;
			}
		}
		//cout << "distanceSecondIt: " << distanceSecondIt << endl << endl;
		return distanceSecondIt;
	}





	void getImmediates(int firstImm, int secondImm)
	{
		if (firstImm <= secondImm)
		{
			for (int i = firstImm; i <= secondImm; ++i)
			{
				positions.emplace_back(polygonPositions[i]);
			}
		}
		else
		{
			for (int i = firstImm; i < polygonPositions.size() - 1; ++i)
			{
				positions.emplace_back(polygonPositions[i]);
			}
			for (int i = 0; i <= secondImm; i++)
			{
				positions.emplace_back(polygonPositions[i]);
			}
		}
	}


	//It's the 2d signed area of the parallelogram. It is not dividing by 2 because I am only interested in its sign. it omits y
	float signedArea(const p3& a, const p3& b, const p3& c);


	bool isPointInTriangle(const p3& a, const p3& b, const p3& c, const p3& p);


	//It has abc as the wave's triangle, l1 and l2 as the polygon segment and p as output
	bool calculateIntersectionPoint(const p3& a, const p3& b, const p3& c, const p3& l1, const p3& l2, p3& p);



};