#pragma once

#include "Camera.h"

class Application;

class Renderer
{
public:
	Renderer() = default;
	Renderer(const unsigned int width, const unsigned int height);
	~Renderer() = default;

	int Init();
	void LateInit();
	void Update();
	void OpenGLClear();
	void UpdateMatrices(mat4x4& projection, mat4x4& view, mat4x4& viewProj);
	void GetShaderUniformLocations(unsigned int shaderLocation);
	void UpdateDeltaTime(double& lastFrame);
	void UpdateTrailTexture();
	void ResetTrailTexture();

	void HandleGameStateImGui();

	void MainMenuStateImGui();
	void GameStateImGui();
	void GameOverStateImGui();
	void VictoryStateImGui();

	Application* application;

	std::shared_ptr<Camera> camera;

	unsigned int screen_width = 0;
	unsigned int screen_height = 0;
	GLFWwindow* window;

	mat4x4 view, translate, rotate, scale, model, viewProj, projection;
	GLint viewProjLocation, modelLocation, viewDirLocation, lightLocation;


	double deltaTime = 0;
	double lastFrame = 0;
	Light ambiantLight;


	//IMGUI

	float gameTimer = 0.0f;
	int wireFrameMode = 0;


};

