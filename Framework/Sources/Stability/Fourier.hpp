#pragma once

#include "Common.hpp"
#include "Time.hpp"

//centered on 0, squared
struct Fourier {

	TimeStruct& tm;
	float length = 100;

	int n = 40; //number of points in one axis of the square grid, minimum 2
	int segments = n - 1;

	float interval = length / segments;



	float period = 40;
	float frecuency = 2 * PI / period;
	float amplitude = 3;
	float offset = 0;
	float phase = 0;
	float phaseSpeed = 0.01;


	vector<unsigned int> indices;
	vector<p3> positions;
	vector<p3> normals;

	unsigned int vertexBuffer;
	unsigned int vertexArray;
	unsigned int indexBuffer;
	unsigned int normalsBuffer;





	bool isBufferUpdated = false;
	GLenum usageHint = GL_DYNAMIC_DRAW;

	Fourier(TimeStruct& tm_):tm(tm_)
	{
		genBuffers();
		createWavePositions();

		/*print(positions);
		print(indices);
		print(normals);*/

		isBufferUpdated = true;
	}
	float waveFunction(float x, float z) {
		//return amplitude * sin(100 * x + phase) * sin(100 * z) + offset;
		//return amplitude * sin(100 * x + phase) + offset;
		return amplitude * sin(frecuency * x - frecuency*tm.currentTime*10);

	}


	//this should only executed at the start
	void createWavePositions() {
		positions.clear();
		positions.reserve(n * n);

		for (float x = 0 - length / 2; x <= 0 + length / 2; x += interval)
		{
			for (float z = 0 - length / 2; z <= 0 + length / 2; z += interval)
			{
				positions.emplace_back(p3{ x,waveFunction(x,z),z });

			}
		}

		createIndices();
		calculateNormals();

		isBufferUpdated = true;
	}

	//indices are created in groups of 4 from one row to the next
	void createIndices()
	{
		indices.clear();
		for (unsigned int i = 0; i < n * (n - 1); i += n)
		{
			for (unsigned int j = 0; j < n - 1; j++)
			{
				indices.insert(indices.end(), { i + j,i + j + n,i + j + 1, i + j + 1,i + j + n,i + j + n + 1 });
			}
		}
	}

	//executed in the while loop
	void updateWavePositions() {
		positions.clear();
		for (float x = 0 - length / 2; x <= 0 + length / 2; x += interval)
		{
			for (float z = 0 - length / 2; z <= 0 + length / 2; z += interval)
			{
				positions.emplace_back(p3{ x,waveFunction(x,z),z });

			}
		}
		isBufferUpdated = true;
	}
	//LOS NORMALES DAN NEGATIVO PORQUE NO ESTÁS HACIENDO ixj SINO ixk
	//MENTIRA, LOS EJES ESTÁN MAL, EL Z NEGATIVO ESTÁ DONDE DEBERÍA ESTAR EL POSITIVO Y NO SÉ PORQUÉ
	void calculateNormals() {

		for (size_t i = 0; i < positions.size(); i += 3)
		{
			p3 v1 = positions[indices[i + 1]] - positions[indices[i]];
			p3 v2 = positions[indices[i + 2]] - positions[indices[i]];

			p3 normal = -normalize3(cross3(v1, v2));
			//print(normal);
			normals.insert(normals.end(), { normal,normal,normal });

		}

	}


	//void createIndices() {
	//	indices.clear();

	//	indices.reserve(segments * segments * 6); //checked

	//	for (int z = 0; z < segments; ++z)
	//	{
	//		for (int x = 0; x < segments; ++x)
	//		{
	//			unsigned int topLeft = (z * n) + x;
	//			unsigned int topRight = topLeft + 1;
	//			unsigned int bottomLeft = topLeft + n;
	//			unsigned int bottomRight = bottomLeft + 1;

	//			indices.insert(indices.end(), { topLeft,bottomLeft,topRight,topRight,bottomLeft,bottomRight });
	//		}
	//	}
	//}

	void draw() {

		glBindVertexArray(vertexArray);

		//the buffer is not expected to be updated at all. Can use data instead of subdata
		if (isBufferUpdated)
		{
			//float stlSize = positions.size() * sizeof(p3);

			glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
			glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(p3), positions.data(), GL_DYNAMIC_DRAW);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_DYNAMIC_DRAW);

			glBindBuffer(GL_ARRAY_BUFFER, normalsBuffer);
			glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(p3), normals.data(), GL_DYNAMIC_DRAW);

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