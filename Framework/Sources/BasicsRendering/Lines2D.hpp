#pragma once

#include "Common.hpp"

//Internet says that the performance diff between glDrawArraysInstanced (for sequencial-nonexistent indices) and glDrawElementsInstanced
//  is negligible although glDrawElementsInstanced having an index loop (and that the same applies without instancing).
//  This should be checked in the distant future 14/02/2025

//POR FAVOR SI PUEDES REVISA LA PIPELINE PARA BUSCAR POSIBLES OPTIMIZACIONES
// , también falta un substitute y dejaría lo de colores para otro momento

//draw lines defined in batches, these batches creates indices for their components
struct Lines2D {

	GLenum usageHint = GL_DYNAMIC_DRAW; //Pass this into the constructor for the final version

	bool isBufferUpdated = false;


	vector<p2> positions;

	//indices must be given in a flat array bc opengl expects it as so. If I need to use the indices at any moment for any othe reason
	// have it in the ui2 format and have a conversion function to flatten it for the gpu
	vector <unsigned int> indices;

	unsigned int vertexBuffer;
	unsigned int vertexArray;
	unsigned int indexBuffer;
	unsigned int instanceVBO;

	//Setting initial buffer size
	size_t currentBufferSize = 0;
	size_t currentDataSize = 0;


	Lines2D() {
		genBuffers();
	}

	


	void genBuffers() {
		glGenVertexArrays(1, &vertexArray);
		glBindVertexArray(vertexArray);

		glGenBuffers(1, &vertexBuffer);
		glGenBuffers(1, &indexBuffer);

		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);

		glBindVertexArray(0);
	}



	void draw() {

		glBindVertexArray(vertexArray);

		//flag activates only when an addSet is Called, otherwise the buffer remains the same
		if (isBufferUpdated)
		{
			currentDataSize = positions.size() * sizeof(p2);

			//resizes both buffers. In this specific case both have the same size
			// positions.size() * sizeof(p) == indices.size() * sizeof(unsigned int)
			if (currentDataSize > currentBufferSize)
			{
				//In the future cap this so it is being multiplied by 2 for small buffers but if it is bigger than some number then only by a 
				// smaller factor 
				currentBufferSize = currentDataSize * 2;

				glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
				glBufferData(GL_ARRAY_BUFFER, currentBufferSize, nullptr, usageHint);

				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, currentBufferSize, nullptr, usageHint);
			}

			//glBufferData creates a bigger buffer than necessary but glBufferSubData sends the exact amount of memory
			glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
			glBufferSubData(GL_ARRAY_BUFFER, 0, currentDataSize, positions.data());

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
			glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, currentDataSize, indices.data());


			isBufferUpdated = false;
		}

		glDrawElements(GL_LINES, indices.size(), GL_UNSIGNED_INT, 0);
	}


	~Lines2D() {
		glDeleteVertexArrays(1, &vertexArray);
		glDeleteBuffers(1, &vertexBuffer);
		glDeleteBuffers(1, &indexBuffer);

		positions.clear(); indices.clear();
	}





	unsigned int indexOffset = 0;	//to add indices over previoses sets

	//Meant to be additive. The mode sets the type of index you want to have
	void addSet(const vector<p2>& items, int mode = 0) {

		positions.reserve(positions.size() + items.size());
		

		positions.insert(positions.end(), items.begin(), items.end());

		createIndices(items);

		isBufferUpdated = true;
	}
	//mode 0
	void createIndices(const vector<p2>& items) {
		indices.reserve(indices.size() + items.size() * 2);
		for (unsigned int i = 0; i < items.size() - 1; i++)
		{
			indices.insert(indices.end(), { indexOffset + i,indexOffset + i + 1 });
		}

		indexOffset = indices.back() + 1;
	}

	void clear() {

		positions.clear(); 
		indices.clear(); 
		indexOffset = 0;
	}
};





