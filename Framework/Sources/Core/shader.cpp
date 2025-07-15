#include "Shader.hpp"


//Converts a .shader into 2 separate streams and each one into strings for glShaderSource in compileShader
ShaderProgramSource Shader::parseShader(const string& filepath) {
	ifstream stream(filepath);

	enum class ShaderType {
		NONE = -1, VERTEX = 0, FRAGMENT = 1
	};

	string line;
	stringstream ss[2];
	ShaderType type = ShaderType::NONE;
	while (getline(stream, line)) {
		if (!line.empty()) //without this check if there's a leading space at the start of the shader it doesn't read the entire shader
		{
			if (line.find("#shader") != std::string::npos) { //npos is the constant that will be given if it was not found, otherwise it will give a position, not a bool
				if (line.find("vertex") != std::string::npos)
					type = ShaderType::VERTEX;
				else if (line.find("fragment") != string::npos)
					type = ShaderType::FRAGMENT;

			}
			else { //what isn't in #shader line is included in the stream
				ss[(int)type] << line << '\n'; //the enum positionates vertex in 0 and fr in 1
			}
		}
	}

	return { ss[0].str(),ss[1].str() }; //str converts streams into strings

}

unsigned int Shader::compileShader(unsigned int type, const string& source) {
	unsigned int id = glCreateShader(type); //you specifies type of shader to create

	const char* src = &source[0];//you will use this pointer to point to the start of your source, char bc string is in chars, equivalent to const char* src = source.c_str()

	glShaderSource(id, 1, &src, nullptr); //Sets the source code in a shader object id 
	//[1] number of elements in the string 
	//[3] if I had an array of source code, this would set the lenght of it to avoid mixing it with other things, I understand 
	glCompileShader(id);

	//Error handling from sintax
	int result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result); //if status of id is successful it will give TRUE
	if (!result) {
		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length); //Gets the length from the error
		//char* message = (char*)alloca(length * sizeof(char)); 
		GLchar message[1024]; //from a example, i think it sets the size of the message, validate
		glGetShaderInfoLog(id, length, &length, message);
		cout << "Failed to compile" << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << endl;
		cout << message << endl;
		glDeleteShader(id);
		return 0;
	}

	return id;
}

unsigned int Shader::createShader(const std::string& vertexShader, const std::string& fragmentShader) {
	unsigned int program = glCreateProgram();// A program object is an object to which shader objects can be attached   and returns a non-zero value by which it can be referenced (unless error, then 0)


	unsigned int vs = compileShader(GL_VERTEX_SHADER, vertexShader);
	compileErrors(vs, "VERTEX SHADER");
	unsigned int fs = compileShader(GL_FRAGMENT_SHADER, fragmentShader);
	compileErrors(fs, "VERTEX SHADER");


	//linking them:
	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);
	glValidateProgram(program);// This validates the program to ensure that the shaders are compatible with the current OpenGL context
	glDeleteShader(vs); // deletes the shader, they are occupying memory with intermediate files
	glDeleteShader(fs);
	return program;
}

// Checks if the different Shaders have compiled properly
void Shader::compileErrors(unsigned int shader, const char* type) {
	// Stores status of compilation
	GLint hasCompiled;
	// Character array to store error message in
	char infoLog[1024];
	if (type != "PROGRAM")
	{
		glGetShaderiv(shader, GL_COMPILE_STATUS, &hasCompiled);
		if (hasCompiled == GL_FALSE)
		{
			glGetShaderInfoLog(shader, 1024, NULL, infoLog);
			std::cout << "SHADER_COMPILATION_ERROR for:" << type << "\n" << infoLog << std::endl;
		}
	}
	else
	{
		glGetProgramiv(shader, GL_LINK_STATUS, &hasCompiled);
		if (hasCompiled == GL_FALSE)
		{
			glGetProgramInfoLog(shader, 1024, NULL, infoLog);
			std::cout << "SHADER_LINKING_ERROR for:" << type << "\n" << infoLog << std::endl;
		}
	}
}


Shader::~Shader() {
	glDeleteProgram(ID);
}
void Shader::bind() {
	glUseProgram(ID);
}

void Shader::unbind() {
	glUseProgram(0);
}









GLFWwindow* initialize() {
	glfwInit();


	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


	glfwWindowHint(GLFW_SAMPLES, 4);  // Request 4x MSAA for antialiasing
	glEnable(GL_MULTISAMPLE);

	glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);
	
	//fullscreen
	//GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "PR0JET R4CC00N", glfwGetPrimaryMonitor(), NULL);
	GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "PR0JET R4CC00N", NULL, NULL);
	
	/*GLFWmonitor* monitor = glfwGetPrimaryMonitor();
	const GLFWvidmode* mode = glfwGetVideoMode(monitor);

	GLFWwindow* window = glfwCreateWindow(mode->width, mode->height, "PR0JET R4CC00N", nullptr, nullptr);
	glfwSetWindowMonitor(window, nullptr, 0, 0, mode->width, mode->height, mode->refreshRate);*/


	/* this will be useful to save width and height
	GLFWmonitor* monitor = glfwGetPrimaryMonitor();
	const GLFWvidmode* mode = glfwGetVideoMode(monitor);
	glfwWindowHint(GLFW_RED_BITS, mode->redBits);
	glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
	glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
	glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

	GLFWwindow* window = glfwCreateWindow(mode->width, mode->height, "PR0JET R4CC00N", NULL, NULL);
	*/


	glfwMakeContextCurrent(window);
	//glewExperimental = GL_TRUE;
	glewInit();
	glfwSwapInterval(1); //0 is vsync disable

	glEnable(GL_LINE_SMOOTH);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);




	return window;
}

void opaque() {
	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
}

//CREO QUE BLENDING NO ES QUE SEA TRANSPARENTE, ES QUE PUEDES CONCATENAR COSAS, QUE SEA TR DEPENDE DE ALPHA
void transparent() {
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//deactivates the depth buffer, if activated the objects behind it won't be rendered
	//, irrelevant if there aren't multiple layers of transparent
	glDepthMask(GL_FALSE);
}

void clearScreen(GlobalVariables gv)
{
	if (gv.program == solar)
	{
		glClearColor(0.035f, 0.065f, 0.085f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}
	else
	{
		glClearColor(40 / 255.0f, 40 / 255.0f, 40 / 255.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}
}