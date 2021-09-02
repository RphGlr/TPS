#define STB_IMAGE_IMPLEMENTATION
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include "stb_image.h"
#include "Texture.h"


void GenerateTexture(unsigned int& texture)
{
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	stbi_set_flip_vertically_on_load(true);

}
void LoadTexture(const char* path, const int channel = 0)
{
	int width, height, nrChannels;
	unsigned char* data = stbi_load(path, &width, &height, &nrChannels, channel);
	if (data != nullptr)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	stbi_image_free(data);

	glBindTexture(GL_TEXTURE_2D, 0);

}

void Texture::GenerateBlankTexture(const int textureWidth, const int textureHeight)
{
	GenerateTexture(textureGLLocation);

	glBindTexture(GL_TEXTURE_2D, textureGLLocation);

	width = textureWidth;
	height = textureHeight;

	const float size = width * height;


	data = new GLubyte[size];

	for (int i = 0; i < size; i++)
	{
		data[i] = 0;
	}


	glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, data);

	glBindTexture(GL_TEXTURE_2D, 0);

}

void Texture::handleTextures(const char* path)
{
	GenerateTexture(textureGLLocation);
	LoadTexture(path);
}