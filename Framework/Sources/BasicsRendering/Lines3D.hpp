#pragma once

#include "Common.hpp"




struct Lines3D {

	GLenum usageHint = GL_DYNAMIC_DRAW; //Pass this into the constructor for the final version

	bool isBufferUpdated = false;

	bool cadMode = false;
	bool isHovering = false;

	vector<p3> positions;

	//indices must be given in a flat array because opengl expects them as such
	vector <unsigned int> indices;

	unsigned int vertexBuffer;
	unsigned int vertexArray;
	unsigned int indexBuffer;

	//Setting initial buffers size
	size_t currentPositionsBufferSize = 0;
	size_t currentIndicesBufferSize = 0;
	size_t currentPositionsDataSize = 0;
	size_t currentIndicesDataSize = 0;


	Lines3D() {
		genBuffers();
	}



	void genBuffers() {
		glGenVertexArrays(1, &vertexArray);
		glBindVertexArray(vertexArray);

		glGenBuffers(1, &vertexBuffer);
		glGenBuffers(1, &indexBuffer);

		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glBindVertexArray(0);
	}









	//One set per lines
	void addSet(const vector<p3>& items, int isConsecutiveIndices = 0) {
		positions.reserve(positions.size() + items.size());

		positions.reserve(items.size());
		positions.insert(positions.end(), items.begin(), items.end());

		//consecutive
		if (isConsecutiveIndices == 0)
		{
			createIndices0(items);
		}
		////pairs
		//else if (isConsecutiveIndices == 2)
		//{
		//	
		//} 

		isBufferUpdated = true;
	}

	unsigned int indexOffset = 0;	//to add indices over previoses sets

	//mode 0 is consecutive, COMPROBAR, RESERVES
	void createIndices0(const vector<p3>& items) {

		for (unsigned int i = 0; i < items.size() - 1; i++)
		{
			indices.insert(indices.end(), { indexOffset + i,indexOffset + i + 1 });
		}

		indexOffset = indices.back() + 1;
	}

	////pairs, NO SÉ QUE ES ESTO, estaba antes de que separara createIndices de addSet
	//void createIndices1(const vector<p2>& items){
	//	indices.clear();
	//	indices.reserve(items.size());
	//	for (unsigned int i = 0; i < items.size() - 1; i += 2)
	//	{
	//		indices.emplace_back(i);
	//		indices.emplace_back(i + 1);
	//	}
	//}

	//SUBSTITUTE IS MISSING

	void draw() {

		glBindVertexArray(vertexArray);

		
		//flag activates only when an addSet is Called, otherwise the buffer remains the same
		//if (isBufferUpdated)
		//{
		//	currentPositionsDataSize = positions.size() * sizeof(p3);
		//	currentIndicesDataSize = indices.size() * sizeof(unsigned int);

		//	//buffers are not equal
		//	if (currentPositionsDataSize > currentPositionsBufferSize)
		//	{
		//		currentPositionsBufferSize = currentPositionsDataSize * 2;
		//		currentIndicesBufferSize = currentIndicesDataSize * 2;

		//		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
		//		glBufferData(GL_ARRAY_BUFFER, currentPositionsBufferSize, nullptr, usageHint);

		//		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
		//		glBufferData(GL_ELEMENT_ARRAY_BUFFER, currentIndicesBufferSize, nullptr, usageHint);
		//	}

		//	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
		//	glBufferSubData(GL_ARRAY_BUFFER, 0, currentPositionsDataSize, positions.data());


		//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
		//	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, currentIndicesDataSize, indices.data());

		//	isBufferUpdated = false;
		//}

		//Me ha dado problemas en cad y le he quitado la optimización
		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(p3), positions.data(), usageHint);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), usageHint);

		glDrawElements(GL_LINES, indices.size(), GL_UNSIGNED_INT, nullptr);

	}

	void clear() {
		positions.clear();
		indices.clear();
		indexOffset = 0;
	}

	~Lines3D() {
		glDeleteVertexArrays(1, &vertexArray);
		glDeleteBuffers(1, &vertexBuffer);
		glDeleteBuffers(1, &indexBuffer);

		positions.clear(); indices.clear();
	}

};


