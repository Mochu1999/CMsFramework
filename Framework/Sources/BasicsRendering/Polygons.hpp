#pragma once

#include "Common.hpp"

#include "SweepTriangulation.hpp"
#include "Lines3D.hpp"


/////////////ESTO ESTÁ TURBO DESACTUALIZADO
struct Polygons {
	GLenum usageHint = GL_DYNAMIC_DRAW;

	bool isBufferUpdated = false;


	Lines3D lines;

	//vector<p3> model;
	vector<p3>& positions = lines.positions;
	vector <unsigned int> indices; //triangle indices, do not mistake them with the lines indices
	SweepTrAux aux; 

	unsigned int vertexBuffer;
	unsigned int vertexArray;
	unsigned int indexBuffer;

	///VECTOR??
	p3 normal;

	//Setting initial buffers size
	size_t currentPositionsBufferSize = 0;
	size_t currentIndicesBufferSize = 0;
	size_t currentPositionsDataSize = 0;
	size_t currentIndicesDataSize = 0;


	

	

	float area = 0.0f;
	p3 centroid;


	Polygons() {
		genBuffers();
	}

	~Polygons();

	void genBuffers();

	void draw();

	void clear();

	void addSet(vector<p3> items);


	void calculateNormal() {
		for (int i = 0; i < positions.size() - 1; i++)
		{
			normal = normalize3(cross3(positions[i + 1] - positions[i], positions[i + 2] - positions[i + 1]));
			if (normal != p3{ 0, 0, 0 }) return;
		}
	}

	





	//void ConvexTriangulation();

	void translate(p3 pos) {
		for (auto& position : positions)
		{
			position += pos;
		}
		isBufferUpdated = true;
	}
	
	void calculateArea() {
		area = 0.0;

		p3 normalSum = { 0, 0, 0 }; // Accumulator for cross products

		
		for (int i = 0; i < positions.size() - 1; ++i)
		{
			// Compute the cross product of consecutive edges and add to normalSum
			normalSum += cross3(positions[i], positions[i + 1]);  // Assuming cross3 returns a p3
		}

		// Compute area as half the magnitude of the accumulated cross products
		area = 0.5 * magnitude3(normalSum);  // Assuming magnitude3 returns a float
	}

	void centroidCalculation() {
		if (area == 0) {
			centroid = positions[0];
			return;
		}

		centroid = { 0, 0, 0 }; // Initialize 3D centroid

		for (int i = 0; i < positions.size() - 1; i++) {
			// Calculate the cross product of consecutive edges to get the area contribution in 3D
			p3 v1 = positions[i];
			p3 v2 = positions[i + 1];
			p3 crossProduct = {
				v1.y * v2.z - v1.z * v2.y,
				v1.z * v2.x - v1.x * v2.z,
				v1.x * v2.y - v1.y * v2.x
			};

			// Use the area contribution (crossProduct) to accumulate centroid coordinates
			float factor = (v1.x * v2.y - v2.x * v1.y);

			centroid.x += (v1.x + v2.x) * factor;
			centroid.y += (v1.y + v2.y) * factor;
			centroid.z += (v1.z + v2.z) * factor;
		}

		// Scale by the total area
		centroid.x *= (1 / (6.0 * area));
		centroid.y *= (1 / (6.0 * area));
		centroid.z *= (1 / (6.0 * area));
	}
};