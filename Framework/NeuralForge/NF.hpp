#pragma once

#include "AuxNF.hpp"

struct NF {
	vector<vector<float>> input;
	vector<vector<vector<float>>> weights;
	vector<vector<float>> biases;

	vector<vector<float>> currentOutput;

	int n_layers;

	NF(vector<vector<float>> input_, vector<int> neuronsPerLayer) :input(input_)
	{
		int currentRows = input[0].size();

		n_layers = neuronsPerLayer.size();
		int n_batches = input.size();

		for (size_t i = 0; i < n_layers; i++) //the last layer of neuronsPerLayer is the output
		{
			weights.push_back(createKaimingWeights(currentRows, neuronsPerLayer[i]));

			vector<float>interm(weights[i][0].size(), 0);
			biases.push_back(interm);

			currentRows = neuronsPerLayer[i];

		}

	};
	void forward() {

		currentOutput = input;
		//print(currentOutput);
		for (size_t i = 0; i < n_layers; i++)
		{

			currentOutput = neuronMult(currentOutput, weights[i], biases[i]);

			if (i < n_layers - 1)
				ReLU(currentOutput);
			else
				softmax(currentOutput);

			//print(currentOutput);
		}


	}
};
