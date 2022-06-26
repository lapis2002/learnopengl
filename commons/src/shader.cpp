#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <shader.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <iostream>

enum ShaderType {
	VERTEX_SHADER,
	FRAGMENT_SHADER
};

Shader::Shader(const char* vertexShaderPath, const char* fragmentShaderPath) {
	// retrieve the vertex/fragment shader source code from the file path
	std::string vertexCode;
	std::string fragmentCode;

	std::ifstream vertexShaderFile;
	std::ifstream fragmentShaderFile;

	// ensure if the ifstream can throw exception
	vertexShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fragmentShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	
	try {
		// open file
		vertexShaderFile.open(vertexShaderPath);
		fragmentShaderFile.open(fragmentShaderPath);

		std::stringstream vertexStream, fragmentStream;

		// read the file buffer content into streams
		vertexStream << vertexShaderFile.rdbuf();
		fragmentStream << fragmentShaderFile.rdbuf();

		// close the file handle
		vertexShaderFile.close();
		fragmentShaderFile.close();

		// convert stream into string
		vertexCode = vertexStream.str();
		fragmentCode = fragmentStream.str();
	}
	catch (std::ifstream::failure e) {
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
	}
	

	const char* vertexShaderCode = vertexCode.c_str();
	const char* fragmentShaderCode = fragmentCode.c_str();
	std::cout << fragmentShaderCode << "\n";
	// compile shader
	unsigned int vertexShader, fragmentShader;

	// vertex shader
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderCode, NULL);
	glCompileShader(vertexShader);

	checkCompileShader(vertexShader, ShaderType::VERTEX_SHADER);

	// fragment shader
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderCode, NULL);
	glCompileShader(fragmentShader);

	checkCompileShader(fragmentShader, ShaderType::FRAGMENT_SHADER);

	// shader program
	id = glCreateProgram();
	glAttachShader(id, vertexShader);
	glAttachShader(id, fragmentShader);

	glLinkProgram(id);

	checkCompileProgram(id);

	// delete shaders as they are linked to program now and are no longer necessary
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}

// -------------------------------------------------------------------------------------------------------------------------------------
void Shader::use() {
	glUseProgram(id);
}

// -------------------------------------------------------------------------------------------------------------------------------------
void Shader::checkCompileShader(unsigned int shader, int shaderType) {
	int success;
	char infoLog[512];
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

	if (!success) {
		glGetShaderInfoLog(shader, 512, NULL, infoLog);
		if (shaderType == ShaderType::VERTEX_SHADER) std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
		else										 std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
}

// -------------------------------------------------------------------------------------------------------------------------------------
void Shader::checkCompileProgram(unsigned int program) {
	int success;
	char infoLog[512];
	glGetProgramiv(program, GL_LINK_STATUS, &success);

	if (!success) {
		glGetProgramInfoLog(program, 512, NULL, infoLog);
		std::cout << "ERROR::PROGRAM::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
}

// -------------------------------------------------------------------------------------------------------------------------------------
void Shader::setBool(const std::string& name, bool value) const {
	glUniform1i(glGetUniformLocation(id, name.c_str()), (int)value);
}

// -------------------------------------------------------------------------------------------------------------------------------------
void Shader::setInt(const std::string& name, int value) const
{
	glUniform1i(glGetUniformLocation(id, name.c_str()), value);
}

// -------------------------------------------------------------------------------------------------------------------------------------
void Shader::setFloat(const std::string& name, float value) const
{
	glUniform1f(glGetUniformLocation(id, name.c_str()), value);
}

// -------------------------------------------------------------------------------------------------------------------------------------
void Shader::setVec3(const std::string& name, float v1, float v2, float v3) const
{
	unsigned int loc = glGetUniformLocation(id, name.c_str());
	glUniform3f(loc, v1, v2, v3);
}
// -------------------------------------------------------------------------------------------------------------------------------------
void Shader::setVec3(const std::string& name, glm::vec3 vec) const
{
	unsigned int loc = glGetUniformLocation(id, name.c_str());
	glUniform3f(loc, vec.x, vec.y, vec.z);
}

// -------------------------------------------------------------------------------------------------------------------------------------
void Shader::setVec4(const std::string& name, float v1, float v2, float v3, float v4) const
{
	unsigned int loc = glGetUniformLocation(id, name.c_str());
	glUniform4f(loc, v1, v2, v3, v4);
}

// -------------------------------------------------------------------------------------------------------------------------------------
void Shader::setMat4(const std::string& name, glm::mat4& value) const
{
	unsigned int loc = glGetUniformLocation(id, name.c_str());
	glUniformMatrix4fv(loc, 1, GL_FALSE, &value[0][0]);
}