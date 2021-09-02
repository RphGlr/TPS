#pragma once
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>



class Shader
{
public:
	Shader(const char* path);
	Shader() = default;

	void shaderDebug();

	unsigned int shaderId;
	std::string shaderS;

};

