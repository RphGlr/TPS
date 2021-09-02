#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <vector>
#include <list>
#include <memory>

#include "SceneManager.h"
#include "Renderer.h"
#include "ResourcesManager.h"
#include "PhysicManager.h"
#include "GameManager.h"

class Application
{
public:
	Application();
	~Application() = default;

	ResourcesManager resourcesManager;
	SceneManager sceneManager;
	Renderer renderer;
	PhysicManager physicManager;
	GameManager gameManager;

	void Init(const unsigned int width, const unsigned int height);
	void LateInit();
	void Update();
	void ResetGame();

#pragma region Singleton
	static Application& getInstance()
	{
		return instance;
	}

private :
	static Application instance;
#pragma endregion

};

