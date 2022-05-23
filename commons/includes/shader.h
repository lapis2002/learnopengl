#pragma once
#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader {
public:
	// the program ID
	unsigned int id;

	// constructor read and build shader
	Shader(const char* vertexShaderPath, const char* fragmentShaderPath);
	
	// activate the shader
	void use();

	// utility uniform functions
	void checkCompileShader(unsigned int shader, int shaderType);
	void checkCompileProgram(unsigned int program);

	void setBool(const std::string &name, bool value) const;
	void setInt(const std::string& name, int value) const;
	void setFloat(const std::string& name, float value) const;

	virtual ~Shader() {
		glDeleteProgram(id);
	}
};



#endif