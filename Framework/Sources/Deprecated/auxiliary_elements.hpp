#pragma once



#include <vector>
using namespace std;


struct Auxiliary_Elements {


	vector<float> positionsOriginal = { 0,0,-50000, 0,0,50000 };
	vector<float>positions;
	vector<unsigned int> indices;
	unsigned int VAAuxiliary_Elements;
	unsigned int VBAuxiliary_Elements;

	float thetaLine;


	void transform(float translation) {
		int nGrid = 100;
		// Draw lines along the z-axis
		for (int i = -nGrid; i < nGrid; i++) {
			positions.insert(positions.end(), {
				positionsOriginal[0] + i * translation, positionsOriginal[1], positionsOriginal[2],
				positionsOriginal[3] + i * translation, positionsOriginal[4], positionsOriginal[5]
				});
		}

		// Draw lines along the x-axis
		for (int i = -nGrid; i < nGrid; i++) {
			positions.insert(positions.end(), {
				positionsOriginal[2], positionsOriginal[1], positionsOriginal[0] + i * translation,
				positionsOriginal[5], positionsOriginal[4], positionsOriginal[3] + i * translation
				});
		}
	}





	Auxiliary_Elements() {
		transform(5);
		Auxiliary_ElementsBuffer();
		CreateAuxiliary_Elementsibo();
		glBindVertexArray(0);
		//for (size_t i = 0; i < positions.size(); i+=3)
		//{
		//	cout << positions[i]<<" "<<positions[i+1]<<" "<<positions[i+2] << endl;
		//}
		//for (size_t i = 0; i < indices.size(); i++)
		//{
		//	cout << indices[i] << endl;
		//}
	}






	void Auxiliary_ElementsBuffer() {
		glBindVertexArray(0);
		glGenVertexArrays(1, &VAAuxiliary_Elements);
		glBindVertexArray(VAAuxiliary_Elements);

		glGenBuffers(1, &VBAuxiliary_Elements);
		glBindBuffer(GL_ARRAY_BUFFER, VBAuxiliary_Elements);
		glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(float), /*positions.data()*/ nullptr, GL_STATIC_DRAW);

		glEnableVertexAttribArray(0); // Attribute index 0
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	}

	void CreateAuxiliary_Elementsibo() {
		for (size_t i = 0; i < positions.size(); i+=3){
			indices.push_back(i/3);
		}

		unsigned int ibo;

		glGenBuffers(1, &ibo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * 4, indices.data(), GL_DYNAMIC_DRAW);

	}


	void draw() {
		glBindVertexArray(0);
		glBindVertexArray(VAAuxiliary_Elements);
		glBindBuffer(GL_ARRAY_BUFFER, VBAuxiliary_Elements);
		glBufferSubData(GL_ARRAY_BUFFER, 0, positions.size() * 4, positions.data());
		glDrawElements(GL_LINES, indices.size(), GL_UNSIGNED_INT, nullptr);
	}



};



