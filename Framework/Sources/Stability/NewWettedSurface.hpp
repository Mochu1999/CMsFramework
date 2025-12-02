#pragma once

#include "Buoy.hpp"
#include "Waves.hpp"
#include "Polyhedra.hpp"


struct WettedBody
{
	vector<p3>& bodyPositions;
	vector<unsigned int>& bodyIndices;
	Waves& wv;

	Polyhedra wet; //wetted surface


	//a vector of coordinates for each body triangle, to keep track of whom each intersection belongs
	// if an intersection does not occur, an empty vector will be pushed
	vector<vector<p3>> intersections; //inner vector<p3> are always triangles
	vector<vector<p3>> bodyTr;
	vector<vector<p3>> allSrfPsts;


	struct Face
	{
		float area = 0;
		p3 centroid;
		p3 n;
		float force = 0;
		p3 forceVec;

		Face(float area_, p3 centroid_, p3 n_, float depth) :area(area_), centroid(centroid_), n(n_)
		{
			force = -1025 * g * depth * area;
			forceVec = n * force;
		}
		
	};
	vector<Face> faces;


	WettedBody(vector<p3>& bodyPositions_, vector<unsigned int>& bodyIndices_, Waves& wv_)
		:bodyPositions(bodyPositions_), bodyIndices(bodyIndices_), wv(wv_)
	{



		calculateWettedBody();

	}


	//The idea is to loop through each triangle, finding every intersection, ordering every intersection, 
	// divide the triangle in two and take the wetted part
	void calculateWettedBody()
	{

		faces.clear();
		intersections.clear();
		bodyTr.clear();
		allSrfPsts.clear();

		for (int i = 0; i < bodyIndices.size(); i += 3)
		{
			p3 r = bodyPositions[bodyIndices[i]];
			p3 s = bodyPositions[bodyIndices[i + 1]];
			p3 t = bodyPositions[bodyIndices[i + 2]];

			bodyTr.insert(bodyTr.end(), { r,s,t });
		}

		calculateIntersections();
		calculateWettedSurfaces();

		wet.clear();



		for (size_t i = 0; i < allSrfPsts.size(); i++)
		{
			//print("a");
			if (allSrfPsts[i].size())
			{
				wet.addPositions(allSrfPsts[i]);
			}
			else
			{

			}
		}

	}



	//It calculates all the intersections at the same time in hope the algorithm ends being executed in the gpu
	//it takes each body triangle and checks it with every wave triangle
	void calculateIntersections()
	{
		for (unsigned int i = 0; i < bodyIndices.size(); i += 3)
		{
			p3 r = bodyPositions[bodyIndices[i]];
			p3 s = bodyPositions[bodyIndices[i + 1]];
			p3 t = bodyPositions[bodyIndices[i + 2]];

			//for bounding box calculation
			p2 minB = { std::min({ r.x,s.x,t.x }),std::min({ r.z,s.z,t.z }) }; //internally the ({...}) are a std::initializer_list<float>
			p2 maxB = { std::max({ r.x,s.x,t.x }),std::max({ r.z,s.z,t.z }) };

			//intersections of each body triangle
			vector<p3> currentIntersections;


			for (unsigned int j = 0; j < wv.indices.size(); j += 3)
			{
				p3 a = wv.positions[wv.indices[j]];
				p3 b = wv.positions[wv.indices[j + 1]];
				p3 c = wv.positions[wv.indices[j + 2]];

				p2 minF = { std::min({ a.x,b.x,c.x }),std::min({ a.z,b.z,c.z }) }; //internally the ({...}) are a std::initializer_list<float>
				p2 maxF = { std::max({ a.x,b.x,c.x }),std::max({ a.z,b.z,c.z }) };

				//bounding box check
				bool overlapX = (maxB.x >= minF.x) && (minB.x <= maxF.x);
				bool overlapZ = (maxB.y >= minF.y) && (minB.y <= maxF.y);
				if (overlapX && overlapZ)
				{
					p3 w, z;
					if (intersectTriangles(r, s, t, a, b, c, w, z))
					{
						currentIntersections.push_back(w);
						currentIntersections.push_back(z);
					}
					else
					{
						//print("a");
					}

				}
				//else if the bounding box fails the for loop continues
			}
			intersections.push_back(currentIntersections);
		}
		//deleting repeated points of intersections
		for (auto& v : intersections) {
			std::sort(v.begin(), v.end(),
				[](auto& a, auto& b) {
					return (a.x != b.x) ? a.x < b.x :
						(a.y != b.y) ? a.y < b.y :
						a.z < b.z;
				});
			v.erase(std::unique(v.begin(), v.end()), v.end());
		}

	}


	//if there's an intersection the bool is true and we retreive w and z
	bool intersectTriangles(p3& r, p3& s, p3& t, p3& a, p3& b, p3& c, p3& w, p3& z)
	{
		p3 n1 = normalize3(cross3(s - r, t - r));      // plane RST
		p3 n2 = normalize3(cross3(b - a, c - a));      // plane ABC

		p3 dir = cross3(n1, n2);
		if (magnitude3(dir) < 1e-6f) return false;     // nearly parallel → ignore

		float d1 = -dot3(n1, r);
		float d2 = -dot3(n2, a);

		p3 A0, A1, B0, B1;

		// segment from ABC cut by plane(RST)
		if (calculateLine(a, b, c, n1, d1, A0, A1) != 2) return false;

		// segment from RST cut by plane(ABC)
		if (calculateLine(r, s, t, n2, d2, B0, B1) != 2) return false;

		// Both segments lie on the same line; take their overlap
		// Parameterize along dir using A0 as anchor.
		auto param = [&](const p3& P) { return dot3(dir, P - A0); };
		float aMin = std::min(param(A0), param(A1));
		float aMax = std::max(param(A0), param(A1));
		float bMin = std::min(param(B0), param(B1));
		float bMax = std::max(param(B0), param(B1));

		float sMin = std::max(aMin, bMin);
		float sMax = std::min(aMax, bMax);
		if (sMin >= sMax) return false;

		// Reconstruct points on the line (dir may be non-unit)
		float invDir2 = 1.0f / dot3(dir, dir);
		w = A0 + dir * (sMin * invDir2);
		z = A0 + dir * (sMax * invDir2);
		return true;
	}


	// returns how many points were found (0, 1, or 2). MVP assumes generic cases → usually 2 or 0.
// n·x + d = 0 is the PLANE OF THE OTHER TRIANGLE.
	int calculateLine(const p3& P0, const p3& P1, const p3& P2, const p3& n, float d, p3& outA, p3& outB)
	{
		auto sd = [&](const p3& p) { return dot3(n, p) + d; };

		float d0 = sd(P0), d1 = sd(P1), d2 = sd(P2);
		int count = 0;

		auto edge = [&](const p3& A, float da, const p3& B, float db)
			{
				// MVP: only handle proper crossings (opposite signs)
				if ((da > 0 && db < 0) || (da < 0 && db > 0))
				{
					float t = da / (da - db);           // t in (0,1)
					p3 I = A + (B - A) * t;
					if (count == 0) outA = I;
					else if (count == 1) outB = I;
					++count;
				}
			};

		edge(P0, d0, P1, d1);
		edge(P1, d1, P2, d2);
		edge(P2, d2, P0, d0);
		return count;
	}

	//After getting the srf points, we calculate the centroid and order the points ccw from any point
	void calculateWettedSurfaces()
	{

		float currentMaxHeight = 0;
		for (unsigned int i = 0; i < bodyTr.size(); i++)
		{
			//4 and 5 are the lateral faces
			if (i < 8)
			{
				//print(i);
				if (intersections[i].size())
				{
					//print(bodyTr[i]);
					p3 a = bodyTr[i][0];
					p3 b = bodyTr[i][1];
					p3 c = bodyTr[i][2];

					p3 n = normalize3(cross3(b - a, c - a));

					vector<p3>intersectionLn = intersections[i];

					p3 aa = intersectionLn[0];
					p3 bb = intersectionLn[0];

					for (const p3& p : intersectionLn) {
						if (p.z < aa.z) aa = p;
						if (p.z > bb.z) bb = p;
					}

					std::vector<p3> reduced = { aa, bb };


					vector<p3> srfPsts = reduced;


					//adding abc to the set of surface points if their .y is smaller than the one from intersectionLn
					if (intersectionLn.size())
					{
						vector<p3> interm = { a,b,c };
						for (p3& i : interm)
						{
							if (i.y < intersectionLn[0].y) srfPsts.push_back(i);
						}
					}
					//print(srfPsts);

					p3 centroid;
					for (auto& p : srfPsts) centroid += p;
					centroid = (1.f / srfPsts.size()) * centroid;
					//print(centroid);

					// build basis in plane
					p3 algo = (fabs(n.x) > 0.9f) ? p3{ 0,1,0 } : p3{ 1,0,0 };
					p3 t = normalize3(cross3(n, algo));
					p3 s = cross3(n, t);

					struct A { p3 p; float ang; };
					std::vector<A> v;
					v.reserve(srfPsts.size());

					for (auto& p : srfPsts) {
						p3 d = p - centroid;
						float x = dot3(d, t), y = dot3(d, s);
						v.push_back({ p, atan2f(y,x) });
					}
					std::sort(v.begin(), v.end(), [](auto& a, auto& b) {return a.ang < b.ang; });

					for (int i = 0; i < srfPsts.size(); ++i) srfPsts[i] = v[i].p;

					//print(srfPsts);

					allSrfPsts.push_back(srfPsts);

					//area calc
					float area = 0.f;
					for (size_t k = 0; k < srfPsts.size(); ++k)
					{
						p3 p0 = srfPsts[k];
						p3 p1 = srfPsts[(k + 1) % srfPsts.size()];
						area += magnitude3(cross3(p0 - centroid, p1 - centroid)) * 0.5f;
					}
					currentMaxHeight = aa.y;
					faces.push_back({ area, centroid, n,currentMaxHeight - centroid.y });
				}
				else
				{
					bool allBelow = true; //only if are all bellow the condition will attach the triangle

					for (const p3& pos : bodyTr[i])
					{
						if (pos.y >= wv.amplitude)
						{
							allBelow = false;
							break;
						}
					}

					if (allBelow) 
					{
						allSrfPsts.push_back(bodyTr[i]);

						p3 a = bodyTr[i][0];
						p3 b = bodyTr[i][1];
						p3 c = bodyTr[i][2];

						p3 n = normalize3(cross3(b - a, c - a));
						float area = 0.5f * magnitude3(cross3(b - a, c - a));
						p3 centroid = (a + b + c) / 3.f;

						faces.push_back({ area, centroid, n,currentMaxHeight - centroid.y });
					}
				}

			}
		}
	}


};