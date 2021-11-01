#pragma once

#ifndef SHADER_H
#define SHADER_H

#include "glad/glad.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader
{
public:
	unsigned int ID;

	Shader(const char* vertexPath, const char* fragmentPath) {
		std::string vertexCode;
		std::string fragmentCode;
		std::ifstream vertexShaderFile;
		std::ifstream fragmentShaderFile;

		vertexShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		fragmentShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

		try {
			// Attempt to read in the shaders from the file paths given.
			vertexShaderFile.open(vertexPath);
			fragmentShaderFile.open(fragmentPath);
			std::stringstream vertexShaderStream;
			std::stringstream fragmentShaderStream;

			vertexShaderStream << vertexShaderFile.rdbuf();
			fragmentShaderStream << fragmentShaderFile.rdbuf();

			vertexShaderFile.close();
			fragmentShaderFile.close();

			vertexCode = vertexShaderStream.str();
			fragmentCode = fragmentShaderStream.str();
		}
		catch(std::ifstream::failure& e){
			std::cout << "File read unsucessful." << std::endl; // Error catching.
		}
		const char* vertexShaderCode = vertexCode.c_str();
		const char* fragmentShaderCode = fragmentCode.c_str();

		unsigned int vertex;
		unsigned int fragment;
		int success;
		char infoLog[512];

		// Create a vertex shader from the vertex source code.
		vertex = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertex, 1, &vertexShaderCode, NULL);
		glCompileShader(vertex);

		glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(vertex, 512, NULL, infoLog);
			std::cout << "Vertex shader compilation failed." << std::endl;
		}

		// Create a fragment shader from the fragment source code.
		fragment = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragment, 1, &fragmentShaderCode, NULL);
		glCompileShader(fragment);

		glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(fragment, 512, NULL, infoLog);
			std::cout << "Fragment shader compilation failed." << std::endl;
		}

		// Create a shader program and link the vertex and fragment shaders.
		ID = glCreateProgram();
		glAttachShader(ID, vertex);
		glAttachShader(ID, fragment);
		glLinkProgram(ID);

		glGetProgramiv(ID, GL_LINK_STATUS, &success);
		if (!success) {
			glGetProgramInfoLog(ID, 512, NULL, infoLog);
			std::cout << "Shader program linking unsuccessful." << std::endl;
		}

		// As the shaders are now linked to the program, deleting is possible.
		glDeleteShader(vertex);
		glDeleteShader(fragment);

		std::cout << "Shader creation successful." << std::endl;
	}

	// Use the shader program.
	void Use() {
		glUseProgram(ID);
	}

	// Send a bool type to a uniform in the shader.
	void SetBool(const std::string& name, bool value) const {
		glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
	}

	// Send an int type to a uniform in the shader.
	void SetInt(const std::string& name, int value) const {
		glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
	}

	// Send a float type to a uniform in the shader.
	void SetFloat(const std::string& name, float value) const {
		glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
	}

	// Send a vec3 type to a uniform in the shader.
	void SetVec3(const std::string& name, float x, float y, float z) const {
		glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
	}
};

#endif