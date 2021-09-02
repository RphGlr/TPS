#include "Application.h"

#include "Input.h"
#include "Player.h"

using namespace Maths;

Renderer::Renderer(const unsigned int width, const unsigned int height) : screen_height{ height }, screen_width{ width }
{
	application = &Application::getInstance();
}

#pragma region Extra Methods

void Renderer::HandleGameStateImGui()
{
	GameManager& gameManager = application->gameManager;

	switch (gameManager.getState())
	{
	case GameState::MAIN_MENU:
		MainMenuStateImGui();
		break;
	case GameState::GAME:
		GameStateImGui();
		break;
	case GameState::GAME_OVER:
		GameOverStateImGui();
		break;
	case GameState::VICTORY:
		VictoryStateImGui();
		break;
	default:
		break;
	}
}

void Renderer::MainMenuStateImGui()
{
	if (ImGui::Begin("MENU"))
	{
		if (ImGui::Button("Play Game"))
		{
			application->gameManager.setState(GameState::GAME);
			application->sceneManager.ChangeScene(0);
		}

		if (ImGui::Button("Quit"))
		{
			glfwSetWindowShouldClose(window, true);
		}
	}
	ImGui::End();
}

void Renderer::GameStateImGui()
{
	gameTimer += deltaTime;

	if (ImGui::Begin("Config"))
	{
		std::shared_ptr<Player> player = application->physicManager.player;

		if (ImGui::CollapsingHeader("Player", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::DragInt("<- Health", &player->health, 0, 3);
			ImGui::DragInt("<- Score", &player->score, 0, INT_MAX);

			ImGui::DragFloat(" <- Timer ", &gameTimer, 0, 300.0f); //5min of gamePlay then Score Board

			Texture hiddenTexture = application->resourcesManager.GetTexture(HiddenTexture);
			Texture detectedTexture = application->resourcesManager.GetTexture(DetectedTexture);

			Texture selectedTexture = player->burningTimer > 0 ? detectedTexture : hiddenTexture;

			ImGui::Image((void*)(intptr_t)selectedTexture.textureGLLocation, ImVec2(200, 128));


		}
	}
	ImGui::End();
}

void Renderer::GameOverStateImGui()
{
	if (ImGui::Begin("Game Over"))
	{
		std::shared_ptr<Player> player = application->physicManager.player;

		ImGui::Text("You died.");
		ImGui::DragFloat("<- You Survived ", &gameTimer, 0, 300.0f); //5min of gamePlay then Score Board

		ImGui::DragInt(" <- You Scored ", &player->score, 0, INT_MAX);
		ImGui::Text("\n");


		if (ImGui::Button("Restart"))
		{
			application->ResetGame();
			application->gameManager.setState(GameState::GAME);
		}

		if (ImGui::Button("Main Menu"))
		{
			application->ResetGame();
			application->gameManager.setState(GameState::MAIN_MENU);
		}

		if (ImGui::Button("Quit"))
		{
			glfwSetWindowShouldClose(window, true);
		}
	}
	ImGui::End();
}

void Renderer::VictoryStateImGui()
{
	if (ImGui::Begin("Victory !"))
	{
		std::shared_ptr<Player> player = application->physicManager.player;

		ImGui::Text("Didn't think you would make it.");
		ImGui::Text("But well ... here's your stats.");


		ImGui::DragInt(" <- Enemy Killed ", &player->score, 0, INT_MAX);
		ImGui::DragInt(" <- You Scored ", &player->score, 0, INT_MAX);
		ImGui::Text("\n");


		if (ImGui::Button("Restart"))
		{
			application->ResetGame();
			application->gameManager.setState(GameState::GAME);
		}

		if (ImGui::Button("Main Menu"))
		{
			application->ResetGame();
			application->gameManager.setState(GameState::MAIN_MENU);
		}

		if (ImGui::Button("Quit"))
		{
			glfwSetWindowShouldClose(window, true);
		}
	}
	ImGui::End();
}

void ProcessInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

#pragma endregion

#pragma region Static Methods

static void DebugMessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
{
	printf("OpenGL error: '%s'\n", message);
}

static void ErrorCallback(int errorCode, const char* description)
{
	printf("GLFW error (code=%d): '%s'\n", errorCode, description);
}

static void EndFrame()
{
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

static void NewFrame()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void drawHealth(float health) {
	glBegin(GL_QUADS);
	glColor3f(1, 0, 0);
	glVertex2f(0, 0);
	glVertex2f(1, 0);
	glVertex2f(1, health);
	glVertex2f(0, health);
	glEnd();
}

static GLFWwindow* InitWindow(int width, int height, const char* title)
{
	// Setup glfw
	glfwSetErrorCallback(ErrorCallback);
	if (!glfwInit())
	{
		printf("glfwInit failed\n");
		return nullptr;
	}

	GLFWmonitor* MyMonitor = glfwGetPrimaryMonitor(); // The primary monitor.. Later Occulus?..

	const GLFWvidmode* mode = glfwGetVideoMode(MyMonitor);
	int SCR_WIDTH = mode->width;
	int SCR_HEIGHT = mode->height;

	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "GLFW test", nullptr, nullptr);
	if (window == nullptr)
	{
		printf("glfwCreateWindow failed\n");
		return nullptr;
	}

	glfwMakeContextCurrent(window);
	glfwSwapInterval(1); // V-Sync
	

	// Setup glad
	if (gladLoadGLLoader((GLADloadproc)glfwGetProcAddress) == 0)
	{
		printf("gladLoaderLoadGL failed\n");
		return nullptr;
	}

	// Setup KHR debug
	if (GLAD_GL_KHR_debug)
	{
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(DebugMessageCallback, NULL);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
		glDebugMessageControl(GL_DONT_CARE, GL_DEBUG_TYPE_PERFORMANCE, GL_DONT_CARE, 0, nullptr, GL_FALSE);
		glDebugMessageControl(GL_DONT_CARE, GL_DEBUG_TYPE_OTHER, GL_DONT_CARE, 0, nullptr, GL_FALSE);
	}

	printf("GL_VENDOR: %s\n", glGetString(GL_VENDOR));
	printf("GL_VERSION: %s\n", glGetString(GL_VERSION));
	printf("GL_RENDERER: %s\n", glGetString(GL_RENDERER));

	// Setup Dear ImGui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable; // Enable Docking

	ImGui::StyleColorsDark();
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0.5));
	ImGuiStyle& style = ImGui::GetStyle();

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 130");

	ImGui::SetCurrentContext(ImGui::GetCurrentContext());
	return window;
}

#pragma endregion

#pragma region Methods

void Renderer::OpenGLClear()
{
	glClearColor(0.0f, 0.0f, 0.5f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::UpdateMatrices(mat4x4& projection, mat4x4& view, mat4x4& viewProj)
{
	projection = camera->getProjection();
	view = camera->getViewMatrix();
	viewProj = projection * view;
}

void Renderer::GetShaderUniformLocations(unsigned int shaderLocation)
{
	viewProjLocation = glGetUniformLocation(shaderLocation, "viewProj");
	modelLocation = glGetUniformLocation(shaderLocation, "model");
	lightLocation = glGetUniformLocation(shaderLocation, "ambiantLight");
	viewDirLocation = glGetUniformLocation(shaderLocation, "viewPos");

	glUniformMatrix4fv(lightLocation, 1, GL_TRUE, &ambiantLight.position.r);
	glUniformMatrix4fv(viewProjLocation, 1, GL_FALSE, viewProj.e);
	glUniform3f(viewDirLocation, camera->transform.position.x, camera->transform.position.y, camera->transform.position.z);
}

void Renderer::UpdateDeltaTime(double& lastFrame)
{
	double currentFrame = glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;
}

void Renderer::UpdateTrailTexture()
{
	Texture* trailTextureLocation = Application::getInstance().sceneManager.trailTexture;
	int textureWidth = trailTextureLocation->width;
	int textureHeight = trailTextureLocation->height;

	glBindTexture(GL_TEXTURE_2D, trailTextureLocation->textureGLLocation);

	float3 playerPosition = application->sceneManager.player->transform.position * 3;

	const float size = textureWidth * textureHeight;

	int indexAtPlayerPos = (int)playerPosition.x * textureWidth + (int)playerPosition.z;

	if (indexAtPlayerPos > size - textureWidth || indexAtPlayerPos < textureWidth) return;
	trailTextureLocation->data[indexAtPlayerPos] = 255;
	trailTextureLocation->data[indexAtPlayerPos + 1] = 255;
	trailTextureLocation->data[indexAtPlayerPos - 1] = 255;
	trailTextureLocation->data[indexAtPlayerPos + textureWidth] = 255;
	trailTextureLocation->data[indexAtPlayerPos - textureWidth] = 255;

	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, trailTextureLocation->width, trailTextureLocation->height, GL_RED, GL_UNSIGNED_BYTE, trailTextureLocation->data);

	glBindTexture(GL_TEXTURE_2D, 0);

}

void Renderer::ResetTrailTexture()
{
	Texture* trailTextureLocation = Application::getInstance().sceneManager.trailTexture;

	int size = trailTextureLocation->width * trailTextureLocation->height;

	for (int i = 0; i < size; i++)
	{
		trailTextureLocation->data[i] = 0;
	}

	glBindTexture(GL_TEXTURE_2D, trailTextureLocation->textureGLLocation);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, trailTextureLocation->width, trailTextureLocation->height, GL_RED, GL_UNSIGNED_BYTE, trailTextureLocation->data);

	glBindTexture(GL_TEXTURE_2D, 0);
}


#pragma endregion

#pragma region Start & Updates

int Renderer::Init()
{
	// glfw window creation
	// --------------------

	window = InitWindow(screen_width, screen_height, "MyLittleRenderer");
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -2;
	}

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	Core::Input::instance()->initialize(window);

	ambiantLight = Light({ 0.f,10.f,0.f,0.1f });
	ambiantLight.position = float4(0, 100, 5000, .5);
	ambiantLight.color = float4(.20, .25, .7, 1);

	return 0;
}

void Renderer::LateInit()
{
	if (camera)
		camera->Init();

	glBindVertexArray(0);

	viewProjLocation = glGetUniformLocation(application->resourcesManager.basicShaderProgram, "viewProj");
	modelLocation = glGetUniformLocation(application->resourcesManager.basicShaderProgram, "model");
	lightLocation = glGetUniformLocation(application->resourcesManager.basicShaderProgram, "ambiantLight");
	viewDirLocation = glGetUniformLocation(application->resourcesManager.basicShaderProgram, "viewPos");

}

void Renderer::Update()
{
	if (!camera)
		return;

	UpdateDeltaTime(lastFrame);

	NewFrame();
	ProcessInput(window);

	camera->Update(deltaTime);

	UpdateMatrices(projection, view, viewProj);

	OpenGLClear();

	application->sceneManager.current_scene->Draw(translate, rotate, scale, model, wireFrameMode);

	HandleGameStateImGui();

	UpdateTrailTexture();

	EndFrame();

	// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
	// -------------------------------------------------------------------------------
	glfwSwapBuffers(window);
	glfwPollEvents();


}

#pragma endregion