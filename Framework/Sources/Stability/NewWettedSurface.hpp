#pragma once





//The wetted body could be calculated with just a function, but I decided to use a struct for better encapsulation, later state storing (saving
// data for the next intersection calculation)


struct WettedBody
{
	Polyhedra wettedBody;

	Polyhedra& body;
	Fourier& mesh;


	//a vector<p3> for each body triangle, to keep track of which body triangle the intersection belongs
		// if intersection does not occur, an empty vector will be pushed
	vector<vector<p3>> intersections; //saco esta variable de calculateWetted body para dibujar lines

	WettedBody(Polyhedra& body_, Fourier& mesh_)
		:body(body_), mesh(mesh_)
	{
		// calculateWettedBody isn't here because when the struct is instanced body is not created yet
	}


	//The idea is to loop through each triangle, finding every intersection, ordering every intersection, 
	// divide the triangle in two and take the wetted part
	void calculateWettedBody()
	{
		intersections.clear();

		/*print(body.positions);
		print(mesh.positions);*/
		calculateIntersections(intersections);

		/*print(intersections.size());
		for (auto& i : intersections)
			print(i);*/
	}

	//It calculates all the intersections at the same time in hope the algorithm ends being executed in the gpu
	void calculateIntersections(vector<vector<p3>>& intersections)
	{
		for (unsigned int i = 0; i < body.indices.size(); i += 3)
		{
			p3 r = body.positions[body.indices[i]];
			p3 s = body.positions[body.indices[i + 1]];
			p3 t = body.positions[body.indices[i + 2]];

			p2 minB = { std::min({ r.x,s.x,t.x }),std::min({ r.z,s.z,t.z }) }; //internally the ({...}) are a std::initializer_list<float>
			p2 maxB = { std::max({ r.x,s.x,t.x }),std::max({ r.z,s.z,t.z }) };

			vector<p3> currentIntersections;

			for (unsigned int j = 0; j < mesh.indices.size(); j += 3)
			{
				p3 a = mesh.positions[mesh.indices[j]];
				p3 b = mesh.positions[mesh.indices[j + 1]];
				p3 c = mesh.positions[mesh.indices[j + 2]];

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
};