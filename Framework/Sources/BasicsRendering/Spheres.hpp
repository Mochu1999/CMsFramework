#pragma once

#include "Common.hpp"
#include "Delaunay2D.hpp"
#include "Lines3d.hpp"

//Creates the points of a Fibonacci Sphere with n points and a defined radius
std::vector<p3> addFibSphere(int n, float radius) {
	std::vector<p3> points;
	points.reserve(n);

	//points are separated horizontally at an angle (the golden angle) from the previous point. 
	const float goldenAngle = PI * (3.0 - std::sqrt(5.0));
	float nInv = 1.0f / n;

	for (int i = 0; i < n; ++i) {
		float y = -1.0 + (2.0 * i + 1.0) * nInv; //Points are distributed vertically from -1 to 1, evenly spaced

		// Calculate radius at this height to the vertical axis to ensure points lie on the sphere surface
		float r = std::sqrt(1.0 - y * y);

		float theta = goldenAngle * i;

		float x = r * std::cos(theta);
		float z = r * std::sin(theta);

		points.emplace_back(p3{ x * radius, y * radius, z * radius });
	}


	return points;
}

std::vector<p2> stereographicProjection(const std::vector<p3>& positions) {
	std::vector<p2> projectedPoints;
	projectedPoints.reserve(positions.size());

	for (const auto& point : positions) {
		// Normalize the point to the unit sphere
		float distance = std::sqrt(point.x * point.x + point.y * point.y + point.z * point.z);
		float nx = point.x / distance;
		float ny = point.y / distance;
		float nz = point.z / distance;


		float projectionScale = 2.0 / (1.0 + ny);  // Standard stereographic projection formula

		p2 projectedPoint;
		projectedPoint.x = projectionScale * nx;

		projectedPoint.y = projectionScale * nz;

		projectedPoints.emplace_back(projectedPoint);
	}

	return projectedPoints;
}


//If you want to draw the mesh lines instead of the triangles, it is currently using Lines3d for it
struct Sphere {
	p3 center;
	float radius;
	unsigned int n;

	//Lines3D lines;

	vector<p3> positions, model;
	vector<p3> normals;

	vector<unsigned int> indices;


	unsigned int vertexBuffer;
	unsigned int vertexArray;
	unsigned int indexBuffer;
	unsigned int normalsBuffer;

	//Setting initial buffers size
	size_t currentPositionsBufferSize = 0;
	size_t currentIndicesBufferSize = 0;
	size_t currentNormalsBufferSize = 0;

	size_t currentPositionsDataSize = 0;
	size_t currentIndicesDataSize = 0;
	size_t currentNormalsDataSize = 0;

	bool isBufferUpdated = false;
	GLenum usageHint = GL_DYNAMIC_DRAW;

	Sphere(float radius_, unsigned int n_ = std::numeric_limits<unsigned int>::max()) : radius(radius_) {
		genBuffers();

		//assigning n if not specified
		if (n_ != std::numeric_limits<unsigned int>::max())
			n = n_;
		else
		{
			n = unsigned int(pow(radius, 2.5));
			n = std::clamp(n, 400u, 10000u);
		}


		model = addFibSphere(n, radius);



		std::vector<p2> projectedPoints = stereographicProjection(model);


		vector<unsigned int> lidIndices; //need to output the indices for the lid for the second iteration
		vector<Triangle> delaunay = bowyerWatson(projectedPoints, lidIndices);



		vector<p2> lidPoints;
		for (const auto& i : lidIndices)
		{
			//already projected
			lidPoints.push_back(p2{ model[i].x,model[i].z });

		}
		vector<Triangle> delaunay2 = bowyerWatson(lidPoints);




		indices = generateTrIndices(projectedPoints, delaunay);
		vector<unsigned int> secondTrIndices = generateTrIndices(lidPoints, delaunay2);
		indices.reserve(indices.size() + secondTrIndices.size());
		indices.insert(indices.end(), secondTrIndices.begin(), secondTrIndices.end());



	}


	//able to be dynamic
	void addSet(p3 center_) {
		positions.clear();
		center = center_;
		positions.resize(model.size(), center);
		for (int i = 0; i < model.size(); i++)
		{
			positions[i] += model[i];
		}
		isBufferUpdated = true;
		//lines.addSet(positions);

		calculateNormals();



		/*print(positions.size());
		print(indices.size() / 3);
		print(normals.size());
		print(positions);
		print(indices);
		print(normals);*/


	}

	//specific for spheres. Normal is the line between vertex and center
	void calculateNormals() {
		normals.clear();


		for (auto& pos : positions)
		{
			normals.push_back(normalize3(pos - center));
		}
	}


	void draw() {

		glBindVertexArray(vertexArray);

		if (isBufferUpdated)
		{
			currentPositionsDataSize = positions.size() * sizeof(p3);
			currentIndicesDataSize = indices.size() * sizeof(unsigned int);

			if (currentPositionsDataSize > currentPositionsBufferSize || currentIndicesDataSize > currentIndicesBufferSize)
			{
				currentPositionsBufferSize = currentPositionsDataSize * 2;
				currentIndicesBufferSize = currentIndicesDataSize * 2;

				glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
				glBufferData(GL_ARRAY_BUFFER, currentPositionsBufferSize, nullptr, usageHint);

				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, currentIndicesBufferSize, nullptr, usageHint);

				glBindBuffer(GL_ARRAY_BUFFER, normalsBuffer);
				glBufferData(GL_ARRAY_BUFFER, currentPositionsBufferSize, nullptr, usageHint);

			}

			glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
			glBufferSubData(GL_ARRAY_BUFFER, 0, currentPositionsDataSize, positions.data());

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
			glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, currentIndicesDataSize, indices.data());

			glBindBuffer(GL_ARRAY_BUFFER, normalsBuffer);
			glBufferSubData(GL_ARRAY_BUFFER, 0, currentPositionsDataSize, normals.data());

			isBufferUpdated = false;
		}



		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, nullptr);

	}

	void genBuffers() {
		glGenVertexArrays(1, &vertexArray);
		glBindVertexArray(vertexArray);

		glGenBuffers(1, &vertexBuffer);
		glGenBuffers(1, &indexBuffer);
		glGenBuffers(1, &normalsBuffer);

		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glBindBuffer(GL_ARRAY_BUFFER, normalsBuffer);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glBindVertexArray(0);
	}

	~Sphere() {
		glDeleteVertexArrays(1, &vertexArray);
		glDeleteBuffers(1, &vertexBuffer);
		glDeleteBuffers(1, &indexBuffer);

		positions.clear(); indices.clear();
	}
};

