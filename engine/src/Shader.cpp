#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <sstream>

#include <stdio.h>
#include <stdlib.h>
#include <vector>


#include "..\include\Shader.h"

using namespace std;


void ParseShaderIntoArray(const char* path, std::string& shader)
{
	ifstream fichier(path, ios::in);
	std::stringstream sstream;
	sstream << fichier.rdbuf();
	shader = sstream.str();
}

Shader::Shader(const char* path) 
{
	ParseShaderIntoArray(path, shaderS);
}

void Shader::shaderDebug()
{
	GLchar infoLogVShader[512];
	GLchar infoLogPShader[512];
	GLint success;
	glGetShaderiv(shaderId, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		char infoLogVShader[500];
		glGetShaderInfoLog(shaderId, 512, NULL, &infoLogVShader[0]);
		std::cout << infoLogVShader << std::endl;
	}

	success = 0;
}

