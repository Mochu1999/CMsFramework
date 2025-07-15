#pragma once

#include "Common.hpp"


struct Fourier {
	

	int n = 20; //number of points in one axis of the square grid
	int segments = n - 1;
	float endPosition = 100;
	float startPosition = 0;
	float interval = (endPosition - startPosition) / segments;




	float period = 3000;
	float frecuency = 2 * PI / period;
	float amplitude = 20;
	float offset = 0;
	float phase = 0;
	float phaseSpeed = 0.01;


	vector<Lines3D> lines;
	vector<unsigned int> indices;
	vector<p3> positions;
	vector<p3> normals;

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

	Fourier() {
		genBuffers();
		//createWavePositions();
		//calculateNormals();
		//createIndices();
		/*cout << "fourier: ";
		printv3(positions);
		printflat(indices);*/
		/*print(positions);
		print(normals);
		print(indices);*/
		
		positions = { {0,0,0},{100,0,0},{100,0,100},{0,0,100} };
		indices = { 0,1,2,0,2,3};
		normals =  {{0,-0.999723,0},{0,-0.999723,0},{0,-0.999723,0},{0,-0.999723,0},{0,-0.999723,0},{0,-0.999723,0} };
		//calculateNormals();
		print(positions.size());
		print(normals.size());
		print(indices.size());

		print(positions);
		print(normals);
		print(indices);
	}
	float waveFunction(float x, float z) {
		//return amplitude * sin(100 * x + phase) * sin(100 * z) + offset;
		//return amplitude * sin(100 * x + phase) + offset;
		return amplitude * sin(100 * x);

	}


	//this should only executed at the start
	void createWavePositions() {
		positions.clear();
		positions.reserve(n * n);

		for (float z = startPosition; z <= endPosition; z += interval)
		{
			for (float x = startPosition; x <= endPosition; x += interval)
			{
				positions.emplace_back(p3{ x,waveFunction(x,z),z });

			}
		}
		phase += phaseSpeed;
		isBufferUpdated = true;


		//lines
		lines.clear();
		lines.resize(indices.size() / 3);
		for (int i = 0; i < indices.size(); i += 3)
		{

			lines[i / 3].addSet({ positions[indices[i]],positions[indices[i + 1]], positions[indices[i + 2]] });
		}
		calculateNormals();
	}

	//executed in the while loop
	void updateWavePositions() {
		size_t index = 0;
		for (float z = startPosition; z < endPosition; z += interval) {
			for (float x = startPosition; x < endPosition; x += interval) {
				positions[index].y = amplitude * sin(100 * x + phase) * sin(100 * z) + offset;
				index++;
			}
		}
		phase += phaseSpeed;
		isBufferUpdated = true;

		calculateNormals();
	}

	void calculateNormals() {
		//for (auto& pos : positions)
		//{

		//// Compute partial derivatives
		//float partialX = amplitude * 100 * cos(100 * pos.x);
		//float partialZ = amplitude * sin(100 * pos.x);

		//p3 vx = normalize3(p3{ 1,0,partialX });  // Tangent vector along x
		//p3 vz = normalize3(p3{ 0,1,partialZ});  // Tangent vector along z

		//// Cross product of vx and vz to get the normal
		//p3 normal = normalize3(cross3(vx, vz));
		//if (normal.y<0)
		//	normals.push_back(p3{ -normal.x, -normal.y, -normal.z });
		//else
		//	normals.push_back(normal);
		//}

		for (size_t i = 0; i < positions.size(); i+=3)
		{
			

			p3 v1 = normalize3(positions[indices[i + 1]] - positions[indices[i]]);
			p3 v2 = normalize3(positions[indices[i + 2]] - positions[indices[i]]);

			p3 normal = normalize3(cross3(v1, v2));
			//print(normal);
			normals.insert(normals.end(), { normal,normal,normal });

		}
		
	}


	void createIndices() {
		indices.clear();

		indices.reserve(segments * segments * 6); //checked
		//indices.insert(indices.end(), { 0,3,1 });
		//print(segments);
		for (int z = 0; z < segments; ++z)
		{
			for (int x = 0; x < segments; ++x)
			{
				unsigned int topLeft = (z * n) + x;
				unsigned int topRight = topLeft + 1;
				unsigned int bottomLeft = topLeft + n;
				unsigned int bottomRight = bottomLeft + 1;

				indices.insert(indices.end(), { topLeft,bottomLeft,topRight,topRight,bottomLeft,bottomRight });
			}
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

	~Fourier() {
		glDeleteVertexArrays(1, &vertexArray);
		glDeleteBuffers(1, &vertexBuffer);
		glDeleteBuffers(1, &indexBuffer);
		glDeleteBuffers(1, &normalsBuffer);
	}
};