#include "IncludesAnchor.hpp"


int main(void)
{
	GlobalVariables gv;
	gv.program = offshoreProgram;

	GLFWwindow* window = initialize(150, 120, "Anchor");



	TimeStruct tm;

	Shader shader3D("resources/shaders/shader3D.shader");
	Shader shader2D("resources/shaders/shader2D.shader");
	Shader shader2D_Instanced("resources/shaders/shader2D_Instanced.shader");
	Shader shaderText("resources/shaders/shaderText.shader");
	Camera camera(window, shader3D, shader2D, shader2D_Instanced, shaderText, gv);



	Anchor anchor(window,gv,tm,shader2D, shaderText);

	/*shader2D.bind();
	shader2D.setUniform("u_Model", identityMatrix);*/

	//SettingsOffshore settings(camera, gv);

	AllPointers allPointers(&camera, &gv, &anchor);
	glfwSetWindowUserPointer(window, &allPointers);
	glfwSetKeyCallback(window, keyboardEventCallback);
	glfwSetMouseButtonCallback(window, mouseEventCallback);
	glfwSetScrollCallback(window, scrollCallback);



	while (!glfwWindowShouldClose(window))
	{
		getPos(window, gv.mPos); 
		if (gv.isRunning)
		{
			tm.update();

			clearScreen(gv);

			anchor.update();
			anchor.draw();





			keyboardRealTimePolls(window, gv, camera);
			camera.updateCamera();
		}


		glfwSwapBuffers(window);
		glfwPollEvents();

	}
	//settings.write();
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}





#pragma once

#include "Common.hpp"
//[[noreturn]] tells the compiler that the function does not return to the caller
[[noreturn]] void terminate_with_error(const std::string& message) {
	std::cerr << "Error: " << message << std::endl;
	std::exit(EXIT_FAILURE);
}

float dot(const std::vector<float>& input1, const std::vector<float>& input2) {

	if (input1.size() != input2.size())
	{
		terminate_with_error("Vectors must be of the same size");
	}

	float output = 0.0f;
	for (size_t i = 0; i < input1.size(); ++i)
	{
		output += input1[i] * input2[i];
	}
	return output;
}


vector<vector<float>> matMult(const vector<vector<float>>& input1, const vector<vector<float>>& input2) {

	if (input1[0].size() != input2.size())
	{
		terminate_with_error("Error: Input1 row size must be equal to input2 column size");
	}

	// Initializes output matrix with zeros and with dimensions [input1.size(),input2[0].size()]
	vector<vector<float>> output(input1.size(), std::vector<float>(input2[0].size(), 0.0f));

	for (size_t i = 0; i < input1.size(); ++i)
	{
		for (size_t j = 0; j < input1[0].size(); ++j)
		{
			for (size_t k = 0; k < input2[0].size(); ++k)
			{
				output[i][k] += input1[i][j] * input2[j][k];
			}
		}
	}
	return output;
}

//TEST IF USING REFERENCES INSIDE LOOPS REDUCES INDEXING OVERHEAD

//Performs the matrix multiplication of inputs and weights, and adds the biases
vector<vector<float>> neuronMult(const vector<vector<float>>& inputs, const vector<vector<float>>& weights, const vector<float>& biases) {

	if (inputs[0].size() != weights.size())
	{
		//number of columns is the size of a row as number of rows is the size of a column
		terminate_with_error("Error: Number of columns in inputs must be equal to number of rows in weights");
	}
	if (weights[0].size() != biases.size())
	{
		terminate_with_error("Error: Biases size must be equal to weights column size");
	}

	// Initializes output matrix with zeros and with dimensions [input1.size(),input2[0].size()]
	vector<vector<float>> output(inputs.size(), std::vector<float>(weights[0].size(), 0.0f));

	for (size_t i = 0; i < inputs.size(); ++i)
	{
		//summing the biases
		for (size_t k = 0; k < weights[0].size(); ++k) {
			output[i][k] = biases[k];
		}

		for (size_t j = 0; j < inputs[0].size(); ++j)
		{
			for (size_t k = 0; k < weights[0].size(); ++k)
			{
				output[i][k] += inputs[i][j] * weights[j][k];
			}
		}
	}
	return output;
}

vector<vector<float>> transpose(const vector<vector<float>>& input) {

	// Initialize output matrix with dimensions [input[0].size()][input.size()]
	vector<vector<float>> output(input[0].size(), std::vector<float>(input.size(), 0.0f));

	for (size_t i = 0; i < input.size(); ++i)
	{
		for (size_t j = 0; j < input[0].size(); ++j)
		{
			output[j][i] = input[i][j];
		}
	}
	return output;
}


//Ideal if ReLU ius been used
std::vector<std::vector<float>> createKaimingWeights(size_t rows, size_t columns) {
	//n_in = rows (neurons previous layer), n_out = columns (neurons current layer)

	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
	std::mt19937 rng(seed);
	//std::mt19937 rng(0);
	float stddev = std::sqrt(2.0f / rows);
	std::normal_distribution<float> dist(0.0f, stddev);

	std::vector<std::vector<float>> weights(rows, std::vector<float>(columns));
	for (size_t i = 0; i < rows; ++i) {
		for (size_t j = 0; j < columns; ++j) {
			weights[i][j] = dist(rng);
		}
	}
	return weights;
}

void ReLU(vector<vector<float>>& input) {
	for (auto& row : input) {
		for (auto& value : row) {
			value = std::max(0.0f, value);
		}
	}
}

void softmax(vector<vector<float>>& input) {
	for (auto& crnt : input)
	{
		//It will be substracting the maximun value to avoid reaching overflow while exponentiating
		float maxValue = *std::max_element(crnt.begin(), crnt.end());

		// Exponentiate each element and compute the sum
		float sum = 0.0f;
		for (auto& i : crnt) {
			i = std::exp(i - maxValue); //all values of i will be between 0 and 1 after the substration
			sum += i;
		}

		// Normalize the exponentials to get probabilities
		for (auto& i : crnt) {
			i /= sum;
		}
	}
}




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
		print(currentOutput);
		for (size_t i = 0; i < n_layers; i++)
		{

			currentOutput = neuronMult(currentOutput, weights[i], biases[i]);

			if (i < n_layers - 1)
				ReLU(currentOutput);
			else
				softmax(currentOutput);

			print(currentOutput);
		}


	}
};



#pragma once

#include "Common.hpp"
//[[noreturn]] tells the compiler that the function does not return to the caller
[[noreturn]] void terminate_with_error(const std::string& message) {
	std::cerr << "Error: " << message << std::endl;
	std::exit(EXIT_FAILURE);
}

float dot(const std::vector<float>& input1, const std::vector<float>& input2) {

	if (input1.size() != input2.size())
	{
		terminate_with_error("Vectors must be of the same size");
	}

	float output = 0.0f;
	for (size_t i = 0; i < input1.size(); ++i)
	{
		output += input1[i] * input2[i];
	}
	return output;
}


vector<vector<float>> matMult(const vector<vector<float>>& input1, const vector<vector<float>>& input2) {

	if (input1[0].size() != input2.size())
	{
		terminate_with_error("Error: Input1 row size must be equal to input2 column size");
	}

	// Initializes output matrix with zeros and with dimensions [input1.size(),input2[0].size()]
	vector<vector<float>> output(input1.size(), std::vector<float>(input2[0].size(), 0.0f));

	for (size_t i = 0; i < input1.size(); ++i)
	{
		for (size_t j = 0; j < input1[0].size(); ++j)
		{
			for (size_t k = 0; k < input2[0].size(); ++k)
			{
				output[i][k] += input1[i][j] * input2[j][k];
			}
		}
	}
	return output;
}

//TEST IF USING REFERENCES INSIDE LOOPS REDUCES INDEXING OVERHEAD

//Performs the matrix multiplication of inputs and weights, and adds the biases
vector<vector<float>> neuronMult(const vector<vector<float>>& inputs, const vector<vector<float>>& weights, const vector<float>& biases) {

	if (inputs[0].size() != weights.size())
	{
		//number of columns is the size of a row as number of rows is the size of a column
		terminate_with_error("Error: Number of columns in inputs must be equal to number of rows in weights");
	}
	if (weights[0].size() != biases.size())
	{
		terminate_with_error("Error: Biases size must be equal to weights column size");
	}

	// Initializes output matrix with zeros and with dimensions [input1.size(),input2[0].size()]
	vector<vector<float>> output(inputs.size(), std::vector<float>(weights[0].size(), 0.0f));

	for (size_t i = 0; i < inputs.size(); ++i)
	{
		//summing the biases
		for (size_t k = 0; k < weights[0].size(); ++k) {
			output[i][k] = biases[k];
		}

		for (size_t j = 0; j < inputs[0].size(); ++j)
		{
			for (size_t k = 0; k < weights[0].size(); ++k)
			{
				output[i][k] += inputs[i][j] * weights[j][k];
			}
		}
	}
	return output;
}

vector<vector<float>> transpose(const vector<vector<float>>& input) {

	// Initialize output matrix with dimensions [input[0].size()][input.size()]
	vector<vector<float>> output(input[0].size(), std::vector<float>(input.size(), 0.0f));

	for (size_t i = 0; i < input.size(); ++i)
	{
		for (size_t j = 0; j < input[0].size(); ++j)
		{
			output[j][i] = input[i][j];
		}
	}
	return output;
}


//Ideal if ReLU ius been used
std::vector<std::vector<float>> createKaimingWeights(size_t rows, size_t columns) {
	//n_in = rows (neurons previous layer), n_out = columns (neurons current layer)

	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
	std::mt19937 rng(seed);
	//std::mt19937 rng(0);
	float stddev = std::sqrt(2.0f / rows);
	std::normal_distribution<float> dist(0.0f, stddev);

	std::vector<std::vector<float>> weights(rows, std::vector<float>(columns));
	for (size_t i = 0; i < rows; ++i) {
		for (size_t j = 0; j < columns; ++j) {
			weights[i][j] = dist(rng);
		}
	}
	return weights;
}

void ReLU(vector<vector<float>>& input) {
	for (auto& row : input) {
		for (auto& value : row) {
			value = std::max(0.0f, value);
		}
	}
}

void softmax(vector<vector<float>>& input) {
	for (auto& crnt : input)
	{
		//It will be substracting the maximun value to avoid reaching overflow while exponentiating
		float maxValue = *std::max_element(crnt.begin(), crnt.end());

		// Exponentiate each element and compute the sum
		float sum = 0.0f;
		for (auto& i : crnt) {
			i = std::exp(i - maxValue); //all values of i will be between 0 and 1 after the substration
			sum += i;
		}

		// Normalize the exponentials to get probabilities
		for (auto& i : crnt) {
			i /= sum;
		}
	}
}




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
		print(currentOutput);
		for (size_t i = 0; i < n_layers; i++)
		{

			currentOutput = neuronMult(currentOutput, weights[i], biases[i]);

			if (i < n_layers - 1)
				ReLU(currentOutput);
			else
				softmax(currentOutput);

			print(currentOutput);
		}


	}
};




