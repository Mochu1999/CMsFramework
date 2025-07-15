#pragma once
#include "Polygons.hpp"
#include "Common.hpp"


//saves positions with its indices
struct StructuredPoints {
	p2 point;
	unsigned int index;

	StructuredPoints() : point({ 0,0 }), index(0) {} //initializing paramenters to 0 if not specified 

	StructuredPoints(p2 point_, unsigned int index_)
		: point(point_), index(index_) {}

};

struct SweepTrAux {

	//I'm letting these outside so I don't create references for the auxiliars of sweep
	std::vector<std::deque<unsigned int>> chain;
	std::vector<StructuredPoints> sPoints;
	vector<unsigned int> indices;
	

	//for 2d, project into x,y if you are in 3d
//triangulates with a sweep algorithm from left to right. It assigns to each point an state and creates and triangulates sequentially chains
	vector<unsigned int> sweepTriangulation(vector<p3> xyPositions/*int i*/) {
		//It can produce triangles of 0 area (collinear sPoints)

		;

		std::vector<unsigned int> edges; //edges are the farthest point of a chain (where the sweep line is)
		std::unordered_set<int> elementsToAvoid;
		std::vector<unsigned int> sortedIndices; //will sort sPoints and store their ordered indices


		sPoints.clear();//structured points, saves positions with its indices in their original order
		chain.clear();
		indices.clear();


		for (unsigned int i = 0; i < xyPositions.size() - 1; i++)
		{
			sPoints.insert(sPoints.end(), { {xyPositions[i].x,xyPositions[i].y},i });
		}


		sortedIndices.resize(sPoints.size());
		std::iota(sortedIndices.begin(), sortedIndices.end(), 0); //initializing it as 0,1,2,3...

		//sorting sortedIndices by x (and greater y if same x) 
		//I could have used [this] but I would be capturing the entire class
		std::sort(sortedIndices.begin(), sortedIndices.end(), [&sPoints = this->sPoints](unsigned int i1, unsigned int i2)
			{
				if (sPoints[i1].point.x == sPoints[i2].point.x)
					return sPoints[i1].point.y > sPoints[i2].point.y;
				return sPoints[i1].point.x < sPoints[i2].point.x;
			});



		/*printflat(sortedIndices);
		cout << "sPoints:" << endl;
		for (const auto& entry : sPoints) {
			cout << entry.index << " " << entry.point.x << " " << entry.point.y<< endl;
		}std::cout << std::endl;*/



		//I enter the pipeline with sPoints (original order) and sorted indices
		for (size_t i = 0; i < sortedIndices.size(); ++i)
		{
			unsigned int currentIndex = sortedIndices[i]; //b[0]

			if (elementsToAvoid.find(currentIndex) == elementsToAvoid.end())  //We avoid "collinears", those that share the same x with out point
			{


				int size = sPoints.size();

				//b is the current point. a is the one that is before b and c the one that is after in cc
				StructuredPoints b = sPoints[currentIndex];
				StructuredPoints a;
				StructuredPoints c;

				//it stores, apart from b, the sPoints that are collinear with b for the same x. All are processed at the same iteration
				std::deque<unsigned int> bContainer;

				//finding a
				int bcounter = 1; //a and c will usually be 1 unit away from b but could be farther if there are more colliner sPoints
				while (true)
				{
					//a = sPoints[(currentIndex - bcounter) % size];    //for i=0, a=size-1 (.back), size-1 % size = size-1
					a = sPoints[(currentIndex - bcounter + size) % size]; //It had to be modified bc C++ doesn't deal with negative modulus

					if (a.point.x == b.point.x)
					{
						elementsToAvoid.emplace(a.index);
						bContainer.push_front(a.index);
						bcounter++;
					}
					else
						break;
				}

				bContainer.push_back(currentIndex);

				//finding c
				bcounter = 1;
				while (true)
				{
					c = sPoints[(currentIndex + bcounter) % size];        //for i=size-1 (.back), size % size = 0

					if (c.point.x == b.point.x)
					{
						elementsToAvoid.emplace(c.index);
						bContainer.push_back(c.index);
						bcounter++;
					}
					else
						break;
				}

				////debug
				//cout << endl << "**** currentIndex = " << currentIndex << endl;

				//cout << "a.index: " << a.index << " c.index: " << c.index << endl;

				//cout << "bContainer: " << endl;
				//for (int i = 0; i < bContainer.size(); i++) {
				//	cout << bContainer[i] << " ";
				//}cout << endl << endl;;



				unsigned int currentChain = 0;
				unsigned int currentEdge = 0;





				//START case, a and c are after b (in the x sweep)
				// Proper if the new pair of edges is above or bellow other edges, improper if it is in between a pair (breaks that pair)
				if (a.point.x > b.point.x && c.point.x > b.point.x)
				{
					bool isProper = true;


					//looking if it is proper or improper (depending on where do you insert the new edges)
					//It passes through all edges and sees if currentIndex is above, in between or below its current edges
					for (int e = 0; e < edges.size(); e += 2)
					{
						//The top check is taking the top point of the chain linked with its edge and the bottom check takes the bottom point of
						//the chain linked with its edge
						float topCheck = isRightOfLine(sPoints[chain[e / 2].front()].point, sPoints[edges[e]].point, sPoints[currentIndex].point);

						float bottomCheck = isRightOfLine(sPoints[chain[e / 2].back()].point, sPoints[edges[e + 1]].point, sPoints[currentIndex].point);


						//above, proper in e
						if (topCheck > 0 && bottomCheck > 0)
						{
							currentEdge = e;
							break;
						}
						//between, improper
						else if (topCheck < 0 && bottomCheck > 0)
						{
							isProper = false;

							currentEdge = e;
							break;

						}
						//below, keeps looping and if there are no more edges, the new ones are created at the bottom
						else if (topCheck < 0 && bottomCheck < 0)
						{
							currentEdge = e + 2; //only used if it's the end of the for loop
						}
					}
					//current chain is the one you were in (for improper is the top chain) 
					// and nextChain will exist only for improper being it the bottom one
					currentChain = currentEdge / 2;
					unsigned int nextChain = currentChain + 1;


					//making space for the new chain (because it's not necessarily at the end)
					chain.resize(chain.size() + 1);


					// moving the elemments greater than currentChain its position + 1
					for (size_t k = chain.size() - 1; k > currentChain; --k)
					{
						chain[k] = std::move(chain[k - 1]);
					}




					//Proper start
					if (isProper)
					{
						chain[currentChain].insert(chain[currentChain].end(), bContainer.begin(), bContainer.end());

						edges.insert(edges.begin() + currentEdge, { a.index,c.index }); //in cc, a will be always above c for proper starts
					}

					//Improper start, it breaks the chain. The top one is currentChain till rm+bContainer 
					//  and the bottom one is a new one formed with bContainer.front and currentChain from rm to back
					else
					{
						//I could store the rightmost point each iteration 
						// or I could calculate it here and only here as I am no using it anywhere else, guess what
						int breakingPoint;
						int maxValue = numeric_limits<int>::min();
						for (int i = 0; i < chain[nextChain].size(); i++) //you end with the rightmost point
						{
							if (sPoints[chain[nextChain][i]].point.x >= maxValue)
							{
								maxValue = sPoints[chain[nextChain][i]].point.x;
								breakingPoint = i;
							}
						}

						//moves nextChain up to the rightmost point to current chain and adds bContainer there
						chain[currentChain].insert(chain[currentChain].end(), chain[nextChain].begin(), chain[nextChain].begin() + breakingPoint + 1);
						chain[currentChain].insert(chain[currentChain].end(), bContainer.begin(), bContainer.end());

						// deletes what remained of current chain in next chain and adds in the front the lowest point of bContainer
						chain[nextChain].erase(chain[nextChain].begin(), chain[nextChain].begin() + breakingPoint);
						chain[nextChain].emplace_front(bContainer.front());



						edges.insert(edges.begin() + currentEdge + 1, { c.index,a.index }); //in cc, c will be always above a for improper starts


						//triangulation could be avoided if we were certain next element was a bend, but not knowing it could be an end

						//top chain
						trChainBack(currentChain);

						//trChainBack(currentChain);


						//bottom chain
						trChainFront(nextChain);
					}
				}



				//END
				else if (b.point.x > a.point.x && b.point.x > c.point.x)
				{
					//theorem, it is a proper end if currentIndex belongs to both elements of a pair of edges. Improper 
					// if they belong to two (different) pairs of edges


					//Con ui2 estarías comparando edges[e].x y edges[e].y y sabrias directamente si hay nextChain o no según donde salga
					for (int e = 0; e < edges.size(); e++)
					{
						if (edges[e] == currentIndex)
						{
							currentEdge = e;
							break;
						}
					}

					currentChain = currentEdge / 2;
					int possibleNextChain = (currentEdge + 1) / 2; //it can be the same as current chain or not



					// Proper end, closes a chain
					if (currentChain == possibleNextChain)
					{
						//if there is collinear first we need to triangulate just up the front of bContainer
						// Only then we apply convex triangulation
						if (bContainer.size() > 1)
						{
							chain[currentChain].emplace_back(bContainer.front());

							trChainBack(currentChain);
						}
						//Deliverately ignoring possible sPoints between bContainer.front and back
						chain[currentChain].emplace_back(bContainer.back());


						//triangulating currentIndex with all the other elements of the cain in a convex triangulation
						for (size_t k = 0; k < chain[currentChain].size() - 2; k++)
						{
							indices.insert(indices.end(), { currentIndex,chain[currentChain][k],chain[currentChain][k + 1] });
						}

						//erase the entire chain and the pair of edges
						chain.erase(chain.begin() + currentChain);
						edges.erase(edges.begin() + currentEdge, edges.begin() + currentEdge + 2);
					}

					// Improper end. Acts as a bend. triangulates two chains and merges them into one. possibleNextChain here is equal to nextChain
					else
					{

						//top chain
						chain[currentChain].emplace_back(currentIndex);

						trChainBack(currentChain);


						//bottom chain
						chain[possibleNextChain].emplace_front(bContainer.back());

						trChainFront(possibleNextChain);


						//merging
						if (chain[currentChain].back() == chain[possibleNextChain].front()) //else there are collinears and you don't delete anything
							chain[possibleNextChain].pop_front();

						chain[currentChain].insert(chain[currentChain].end(),
							std::make_move_iterator(chain[possibleNextChain].begin()),
							std::make_move_iterator(chain[possibleNextChain].end()));

						chain.erase(chain.begin() + possibleNextChain); //erase the chain

						edges.erase(edges.begin() + currentEdge, edges.begin() + currentEdge + 2);
					}
				}



				//BEND
				else
				{
					//Looking if the bend occurs at the top or the bottom
					for (int e = 0; e < edges.size(); ++e)
					{
						if (bContainer.back() == edges[e] || bContainer.front() == edges[e])
						{
							currentEdge = e;
							break;
						}
					}


					currentChain = currentEdge / 2;

					// changing edges to the immediate of bigger x
					if (a.point.x > c.point.x)
						edges[currentEdge] = a.index;
					else
						edges[currentEdge] = c.index;



					//bend at the top edge
					if (currentEdge % 2 == 0)
					{
						//if there is one item in bContainer or more than one it will add .back to its front and triangulate
						chain[currentChain].emplace_front(bContainer.back());
						trChainFront(currentChain);

						if (bContainer.size() > 1)
						{
							//It adds the collinear but only triangulates if the collinear is bellow b
							chain[currentChain].emplace_front(bContainer.front());

							if (sPoints[bContainer.front()].point.y < sPoints[bContainer.back()].point.y)
								trChainFront(currentChain);
						}
					}

					//bend at the bottom edge
					else
					{
						chain[currentChain].emplace_back(bContainer.front());
						trChainBack(currentChain);

						if (bContainer.size() > 1)
						{
							//It adds the collinear but only triangulates if the collinear is above b
							chain[currentChain].emplace_back(bContainer.back());

							if (sPoints[bContainer.front()].point.y < sPoints[bContainer.back()].point.y)
								trChainBack(currentChain);
						}
					}
				}

				//cout << "chain:" << endl;
				//for (const auto& row : chain) {
				//	for (const auto& value : row) {
				//		std::cout << value << " ";
				//	}cout << endl;
				//}cout << endl;


				//cout << "edges:" << endl;
				//for (const auto& value : edges) {
				//	std::cout << value << " ";
				//}cout << endl << endl;

				///*cout << "elementsToAvoid:" << endl;
				//for (int elem : elementsToAvoid) {
				//	std::cout << elem << " ";
				//}
				//std::cout << std::endl << endl;*/

				//cout << "indices:" << endl;
				//for (int i = 0; i < indices.size(); i += 3) {
				//	cout << indices[i] << " ";
				//	cout << indices[i + 1] << " ";
				//	cout << indices[i + 2] << endl;
				//}
				//cout << endl;

			}
		}
		return indices;
	}

	//auxiliar functions for sweepTriangulation

	//the new point is added at the back and we are triangulation from that one to till the start of the chain
	void trChainBack(const unsigned int& currentChain) {

		auto backIndex = chain[currentChain].back();
		auto PenultimateIndex = chain[currentChain][chain[currentChain].size() - 2];

		for (int k = chain[currentChain].size() - 3; k >= 0; k--)
		{
			if (crossProduct(sPoints[backIndex].point,
				sPoints[PenultimateIndex].point,
				sPoints[chain[currentChain][k]].point) <= 0)
				//<0 because order is clockwise (result still cc), =0 bc the area can be 0 in collinear and need to erase
			{
				indices.insert(indices.end(), { chain[currentChain][k],PenultimateIndex,backIndex });

				chain[currentChain].erase(chain[currentChain].begin() + k + 1);
				//I don't need elementsToErase if the order is from end to start in the for loop

				PenultimateIndex = chain[currentChain][k];
			}

		}
	}

	//the new point is added at the front and we are triangulation from that one to till the end of the chain
	void trChainFront(const unsigned int& currentChain) {

		if (chain[currentChain].size() > 2)
		{
			vector<int> elementsToErase; //separating them to remove them from in a backwards motion
			elementsToErase.reserve(5);

			auto frontIndex = chain[currentChain].front();
			auto secondIndex = chain[currentChain][1];

			for (size_t k = 2; k < chain[currentChain].size(); k++)
			{
				if (crossProduct(sPoints[frontIndex].point,
					sPoints[secondIndex].point,
					sPoints[chain[currentChain][k]].point) > 0)
				{
					indices.insert(indices.end()
						, { frontIndex,secondIndex,chain[currentChain][k] });

					elementsToErase.emplace_back(k - 1);

					secondIndex = chain[currentChain][k];
				}

			}

			for (int k = elementsToErase.size() - 1; k >= 0; k--)
			{
				chain[currentChain].erase(chain[currentChain].begin() + elementsToErase[k]);
			}

		}
	}
};