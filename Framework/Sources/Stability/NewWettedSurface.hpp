#pragma once

#include "Buoy.hpp"
#include "Waves.hpp"




struct WettedBody
{
	Polyhedra& body;
	Polyhedra wet; //wetted surface
	Waves& wv;


	//a vector of coordinates for each body triangle, to keep track of whom each intersection belongs
	// if an intersection does not occur, an empty vector will be pushed
	vector<vector<p3>> intersections; //inner vector<p3> are always triangles
	vector<vector<p3>> wetTriangles; //

	WettedBody(Buoy& buoy_, Waves& wv_)
		:body(buoy_.body), wv(wv_)
	{
		calculateWettedBody();

		for (unsigned int i = 0; i < body.indices.size(); i += 3)
		{
			p3 r = body.positions[body.indices[i]];
			p3 s = body.positions[body.indices[i + 1]];
			p3 t = body.positions[body.indices[i + 2]];

			vector<p3> interm;
			interm.insert(interm.end(), { r,s,t });

			print(interm);

			print(intersections[i / 3]);
		}
	}


	//The idea is to loop through each triangle, finding every intersection, ordering every intersection, 
	// divide the triangle in two and take the wetted part
	void calculateWettedBody()
	{
		intersections.clear();

		/*print(body.positions);
		print(wv.positions);*/
		calculateIntersections();
		calculateWettedSurfaces();

	}

	

	//It calculates all the intersections at the same time in hope the algorithm ends being executed in the gpu
	//it takes each body triangle and checks it with every wave triangle
	void calculateIntersections()
	{
		for (unsigned int i = 0; i < body.indices.size(); i += 3)
		{
			p3 r = body.positions[body.indices[i]];
			p3 s = body.positions[body.indices[i + 1]];
			p3 t = body.positions[body.indices[i + 2]];

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
	int calculateLine(const p3& P0, const p3& P1, const p3& P2,
		const p3& n, float d,
		p3& outA, p3& outB)
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


	void calculateWettedSurfaces()
	{
		for (unsigned int i = 0; i < body.indices.size(); i += 3)
		{
			break; //leaving this idea for the moment
			p3 a = body.positions[body.indices[i]];
			p3 b = body.positions[body.indices[i + 1]];
			p3 c = body.positions[body.indices[i + 2]];

			vector<p3> interm;
			interm.insert(interm.end(), { a,b,c }); //solo para print

			print(interm);

			vector<p3>intersectionLn = intersections[i / 3];
			print(intersectionLn);

			p3 n = normalize3(cross3(b - a, c - a));

			//// up vector not parallel to n
			//p3 up = fabs(n.y) < 0.9f ? p3{ 0,1,0 } : p3{ 1,0,0 };

			//p3 t = normalize3(cross3(n, up)); // tangent direction on triangle plane

			//std::sort(intersectionLn.begin(), intersectionLn.end(),
			//	[&](const p3& p1, const p3& p2)
			//	{
			//		float s1 = dot3(p1, t);
			//		float s2 = dot3(p2, t);
			//		return s1 > s2; // > gives "right to left"
			//	});
		}
		for (unsigned int i = 0; i < body.indices.size(); i += 3)
		{
			if (i /= 3 < 4)
			{
				p3 a = body.positions[body.indices[i]];
				p3 b = body.positions[body.indices[i + 1]];
				p3 c = body.positions[body.indices[i + 2]];

				vector<p3>intersectionLn = intersections[i / 3];

				vector<p3> allSrfPsts = intersectionLn;
				
				if (a.y < intersectionLn[0].y) allSrfPsts.push_back(a);
				if (b.y < intersectionLn[0].y) allSrfPsts.push_back(b);
				if (c.y < intersectionLn[0].y) allSrfPsts.push_back(c);

			}
		}
	}
	static p3 centroidAndSort(std::vector<p3>& pts, const p3& n)
	{
		int m = pts.size();
		p3 c{ 0,0,0 };
		for (auto& p : pts) c = c + p;
		c = (1.f / m) * c;

		// build basis in plane
		p3 a = (fabs(n.x) > 0.9f) ? p3{ 0,1,0 } : p3{ 1,0,0 };
		p3 t = normalize3(cross3(n, a));
		p3 s = cross3(n, t);

		struct A { p3 p; float ang; };
		std::vector<A> v; v.reserve(m);
		for (auto& p : pts) {
			p3 d = p - c;
			float x = dot3(d, t), y = dot3(d, s);
			v.push_back({ p, atan2f(y,x) });
		}
		std::sort(v.begin(), v.end(), [](auto& a, auto& b) {return a.ang < b.ang; });

		for (int i = 0; i < m; ++i) pts[i] = v[i].p;
		return c;
	}

};