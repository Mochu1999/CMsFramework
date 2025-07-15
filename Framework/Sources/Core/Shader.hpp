#pragma once

#include "Common.hpp"

//Había un void bind() const {} que hay que reexplorar



// to not render what is not visible to the camera:
//glEnable(GL_CULL_FACE);       // Enable face culling
//glCullFace(GL_BACK);          // Cull back faces
//glFrontFace(GL_CCW);          // Counter-clockwise winding is front-facing


struct ShaderProgramSource {
	std::string VertexSource;
	std::string FragmentSource;
};

struct Shader {

	std::string filePath;
	unsigned int ID;



	Shader(const std::string& filepath)
		:filePath(filepath), ID(0) { //we are maintaining filePath only for debugging purpouses
		ShaderProgramSource source = parseShader(filepath);
		ID = createShader(source.VertexSource, source.FragmentSource);

		
	}

	//Converts a .shader into 2 separate streams and each one into strings for glShaderSource in compileShader
	ShaderProgramSource parseShader(const string& filepath);


	unsigned int compileShader(unsigned int type, const string& source);

	unsigned int createShader(const std::string& vertexShader, const std::string& fragmentShader);
	


	// Checks if the different Shaders have compiled properly
	void compileErrors(unsigned int shader, const char* type);

	~Shader();
	void bind();
	void unbind();

	////////////////////////////
	///Look for uniform buffer objects. Used to send a lot of uniforms into a program more efficiently.
	// I think it let's you share DATA BETWEEN SHADERS


	std::unordered_map<std::string, int> locationsMap;

	//If there's no location it will be created, otherwise it will take it from map
	//DOES A MAP ELIMINATE THE IF??
	int getUniformLocation(const std::string& name) {

		//if it is in the map
		auto it = locationsMap.find(name);
		if (it != locationsMap.end()) {
			return it->second;  // Slightly faster than return locationsMap[name], only one lookup is performed
		}
		//else
		int location = glGetUniformLocation(ID, name.c_str());
		locationsMap[name] = location;
		return location;
	}

	//Change to Templates////
	//1f
	void setUniform(const std::string& name, float value) {
		glUniform1f(getUniformLocation(name), value);
	}

	//3f
	void setUniform(const std::string& name, float x, float y, float z) {
		glUniform3f(getUniformLocation(name), x, y, z);
	}
	//p3
	void setUniform(const std::string& name, const p3& vec) {
		glUniform3f(getUniformLocation(name), vec.x, vec.y, vec.z);
	}
	//4f
	void setUniform(const std::string& name, float x, float y, float z, float w) {
		glUniform4f(getUniformLocation(name), x, y, z, w);
	}
	//1i
	void setUniform(const std::string& name, int value) {
		glUniform1i(getUniformLocation(name), value);
	}
	//mat4
	void setUniform(const std::string& name, const std::array<float, 16>& mat) {
		glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, mat.data());
	}
};



//Initializes glfw and glew
GLFWwindow* initialize();

void opaque();
void transparent();

void clearScreen(GlobalVariables gv);


//#define CHECK_GL_ERROR() {\
//    GLenum err = glGetError();\
//    while (err != GL_NO_ERROR) {\
//        std::cout << "OpenGL error: " << std::hex << err << std::dec << " line:" << __LINE__ << std::endl;\
//        err = glGetError();\
//    }\
//}
//std::set<std::string> printedErrors;
//
//void GLAPIENTRY MessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) { //Only adds errors once
//	std::string errorMsg(message);
//
//	if (printedErrors.find(errorMsg) == printedErrors.end()) {
//		std::cerr << message << std::endl;
//
//		printedErrors.insert(errorMsg);
//	}
//}