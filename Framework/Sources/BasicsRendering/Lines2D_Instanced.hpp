#pragma once

#include "Common.hpp"

struct InstanceAttributes {
	p2 translation;
	float cosTheta;
	float sinTheta;
	p2 scale;

	InstanceAttributes(p2 translation_, float angle_, p2 scale_)
		: translation(translation_),
		cosTheta(cos(angle_)),
		sinTheta(sin(angle_)),
		scale(scale_) {}
};

//In instance drawing you have one single set to draw multiple times with addSet, which here, isn't accumulative
struct Lines2D_Instanced {

	GLenum usageHint = GL_DYNAMIC_DRAW; //Pass this into the constructor for the final version

	bool isBufferUpdated = false;

	

	vector<p2> positions;
	vector <unsigned int> indices;
	vector<InstanceAttributes> instances;

	unsigned int vertexArray;
	unsigned int vertexBuffer;
	unsigned int indexBuffer;
	unsigned int instanceBuffer;


	//Setting initial buffer size
	size_t currentBufferSize = 0;
	size_t currentDataSize = 0;


	




	Lines2D_Instanced() {
		genBuffers();
	}


	//REVISA
	void genBuffers() {
		glGenVertexArrays(1, &vertexArray);
		glBindVertexArray(vertexArray);

		glGenBuffers(1, &vertexBuffer);
		glGenBuffers(1, &indexBuffer);
		glGenBuffers(1, &instanceBuffer);


		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);

		glBindBuffer(GL_ARRAY_BUFFER, instanceBuffer);
		glBufferData(GL_ARRAY_BUFFER, instances.size() * sizeof(InstanceAttributes),instances.data(), GL_DYNAMIC_DRAW);

		// Attribute for translation (vec2) at location 1:
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(InstanceAttributes), (void*)0);
		glVertexAttribDivisor(1, 1);  // Advance once per instance

		// Attribute for rotation (vec2: cosTheta, sinTheta) at location 2:
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(InstanceAttributes),(void*)(sizeof(p2)));  // offset = sizeof(p2) (translation)
		glVertexAttribDivisor(2, 1);

		// Attribute for scale (vec2) at location 3:
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(InstanceAttributes),(void*)(sizeof(p2) + 2 * sizeof(float)));  // offset after translation and rotation
		glVertexAttribDivisor(3, 1);
		glBindVertexArray(0);
	}

	//////////////////////////////////////////////////
	//OPTIMIZE////////////////////////////////////////
	void draw() {
		glBindVertexArray(vertexArray);

		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(p2), positions.data(), usageHint);

		glBindBuffer(GL_ARRAY_BUFFER, instanceBuffer);
		glBufferData(GL_ARRAY_BUFFER, instances.size() * sizeof(InstanceAttributes), instances.data(), usageHint);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), usageHint);


		glDrawElementsInstanced(GL_LINES, indices.size(), GL_UNSIGNED_INT, 0, instances.size());
	}



	~Lines2D_Instanced() {
		glDeleteVertexArrays(1, &vertexArray);
		glDeleteBuffers(1, &vertexBuffer);
		glDeleteBuffers(1, &indexBuffer);
		glDeleteBuffers(1, &instanceBuffer);

		positions.clear(); indices.clear();
	}






	//A single set
	void addSet(const vector<p2>& items, int mode = 0) {
		positions.clear();
		indices.clear();

		positions.reserve(items.size());
		indices.reserve(items.size() * 2);

		positions.insert(positions.end(), items.begin(), items.end());

		createIndices(items);

		isBufferUpdated = true;
	}

	//mode 0
	void createIndices(const vector<p2>& items) {
		for (unsigned int i = 0; i < items.size() - 1; i++)
		{
			indices.insert(indices.end(), {i,i + 1 });
		}
	}

	//Substitutive
	void addInstances(const vector<InstanceAttributes> intances_) {
		instances.clear();
		instances = intances_;
	}

	void clear() {

		positions.clear();
		indices.clear();
		instances.clear();
	}
};