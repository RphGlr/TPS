#pragma once
#include <vector>
#include <deque>

#include "Shader.h"
#include "Model.h"

enum ModelEnum
{
	Cube = 0,
	Ball = 1,
	Plane = 2,
	Sphere = 3
};
enum TextureEnum
{
	BlankTexture = 0,
	CollisionTexture = 1,
	TrailTexture = 2,
	FOVTexture = 3,

	HiddenTexture = 4,
	DetectedTexture = 5

};

enum ShaderEnum
{
	TerrainShaderProgram = 3,
	PlayerShaderProgram = 6,
	SkyBoxShaderProgram = 9,
	BrickShaderProgram = 12,
	EnemyShaderProgram = 16
};

class ResourcesManager
{
public:
	ResourcesManager() = default;
	~ResourcesManager() = default;

	int Init();
	void Update();

	void AddModels();
	void AddTextures();
	void AddShaders();

	Model GetModel(int iD);
	Texture& GetTexture(int iD);
	unsigned int GetShader(int iD);

	unsigned int basicShaderProgram = -1;

	unsigned int CreateShaderProgram(const char* fragmentShaderPath, const char* vertexShaderPath);
	unsigned int CreateGeoShaderProgram(const char* fragmentShaderPath, const char* vertexShaderPath, const char* geoShaderPath);

	std::deque<Model> models;
	std::deque<Texture> textures;
	std::deque<unsigned int> shaders;

};