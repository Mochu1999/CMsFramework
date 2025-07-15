#include "Polygons.hpp"

void Polygons::genBuffers() {
	glGenVertexArrays(1, &vertexArray);
	glBindVertexArray(vertexArray);

	glGenBuffers(1, &vertexBuffer);
	glGenBuffers(1, &indexBuffer);

	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	//glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);

	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindVertexArray(0);
}

void Polygons::draw() {
	GLenum usageHint = GL_DYNAMIC_DRAW;

	glBindVertexArray(vertexArray);

	////flag activates only when an addSet is Called, otherwise the buffer remains the same
	if (isBufferUpdated)
	{
		currentPositionsDataSize = positions.size() * sizeof(p3);
		currentIndicesDataSize = indices.size() * sizeof(unsigned int);

		//Estoy haciendo pruebas y he tenido que poner la nueva condición. No creo que sea necesaria en el futuro
		if (currentPositionsDataSize > currentPositionsBufferSize || currentIndicesDataSize > currentIndicesBufferSize)
		{

			currentPositionsBufferSize = currentPositionsDataSize * 2;
			currentIndicesBufferSize = currentIndicesDataSize * 2;

			glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
			glBufferData(GL_ARRAY_BUFFER, currentPositionsBufferSize, nullptr, usageHint);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, currentIndicesBufferSize, nullptr, usageHint);
		}

		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
		glBufferSubData(GL_ARRAY_BUFFER, 0, currentPositionsDataSize, positions.data());


		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
		glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, currentIndicesDataSize, indices.data());

		isBufferUpdated = false;
	}
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, nullptr);
}


Polygons::~Polygons() {
	glDeleteVertexArrays(1, &vertexArray);
	glDeleteBuffers(1, &vertexBuffer);
	glDeleteBuffers(1, &indexBuffer);
}


void Polygons::addSet(vector<p3> items) {

	lines.addSet(items);
	//model = positions;

	//sweepTriangulation();

	calculateNormal();
	calculateArea();


	/*if (area < 0) {
		for (int i = 0; i < positions.size() / 2; i += 2) {
			std::swap(positions[i], positions[positions.size() - 2 - i]);
			std::swap(positions[i + 1], positions[positions.size() - 1 - i]);
		}
		areaCalculation();
	}*/

	centroidCalculation();
	//polarAreaMomentOfInertia();

	//print(area);
	//print(centroid);
	//the variable is rotated in addSet so the z value is equal for every point
	vector<p3> xyPositions = positions;

	//The objective is to set in xyPositions with the faces rotated to allign with the normal {0,0,1}
	
	//Finds the rotation axis
	p3 rotationAxis = cross3(normal, { 0,0,1 });
	//Finds the angle of rotation (dot/magnitude)
	float angle = acos(dot3(normal, { 0,0,1 }) / (magnitude3(normal) * magnitude3(p3{ 0,0,1 }))); // angle in radians


	for (auto& pos : xyPositions)
	{
		pos -= centroid;
		rotatePoint(pos, angle, rotationAxis);
		pos += centroid;
	}

	isBufferUpdated = true;
	indices = aux.sweepTriangulation(xyPositions);
	//calculateNormal();
}






//void Polygons::ConvexTriangulation() {
//
//	for (unsigned int i = 1; i < positions.size() - 2; i++) {
//		indices.insert(indices.end(), { 0, i, i + 1 });
//	}
//
//}




void Polygons::clear() {

	indices.clear();
}

