#pragma once
#include "Types.h"

class Texture
{
public:
	Texture() = default;
	~Texture() = default;

	void handleTextures(const char* path);
	void GenerateBlankTexture(const int width, const int height);
	unsigned int textureGLLocation;
	int textureiD;
	GLubyte* data;

	int width;
	int height;
};