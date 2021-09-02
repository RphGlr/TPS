#include "Application.h"
#include <time.h> 

Application Application::instance = Application();

Application::Application()
{
}

void Application::Init(const unsigned  int scr_width, const unsigned int scr_height)
{
	sceneManager.Init();

	renderer = Renderer(scr_width, scr_height);

	renderer.camera = sceneManager.current_scene->hierarchy.back().GetComponent<Camera>();

	renderer.Init();

	physicManager.Init();

	resourcesManager.Init();
}

void Application::LateInit()
{
	sceneManager.LateInit(&resourcesManager);

	renderer.LateInit();
}

void Application::Update()
{
	while (!glfwWindowShouldClose(renderer.window))
	{
		sceneManager.Update(); //1
		renderer.Update(); //2
		physicManager.Update(renderer.deltaTime);
		resourcesManager.Update();
	}	
	glfwTerminate();
}

void Application::ResetGame()
{
	physicManager.player->Reset();
	physicManager.ResetEnemies();
	
	renderer.ResetTrailTexture();

	renderer.gameTimer = 0;
}
