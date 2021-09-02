#include "Application.h"

void LoadModel(std::deque<Model>& models, const char* objPath, Texture texture, int& iD ,float3 position = { 0,0,0 })
{
	Model m = Model(objPath, texture, position, iD);
	iD++;

	models.push_back(m);
}
void LoadTexture(std::deque<Texture>& textures, const char* texturePath, int& iD)
{
	Texture t;
	t.handleTextures(texturePath);
	t.textureiD = iD;
	iD++;

	textures.push_back(t);
}

Texture* CreateBlankTexture(std::deque<Texture>& textures, int& iD, const int width, const int height)
{
	Texture t;
	t.GenerateBlankTexture(width, height);
	t.textureiD = iD;
	iD++;

	textures.push_back(t);

	return &textures.back();

}
void ResourcesManager::AddModels()
{
	int modelId = 0;

	Texture basicBlankTexture = GetTexture(BlankTexture);


	LoadModel(models, "Resources/obj/Cube.obj", basicBlankTexture,  modelId);
	LoadModel(models, "Resources/obj/ball.obj", basicBlankTexture,  modelId);
	LoadModel(models, "Resources/obj/Plane.obj", basicBlankTexture, modelId);
	LoadModel(models, "Resources/obj/Sphere.obj", basicBlankTexture, modelId);

}
void ResourcesManager::AddTextures()
{
	int textureId = 0;

	CreateBlankTexture(textures, textureId, 1000, 1000);

	Application::getInstance().sceneManager.renderTexture = CreateBlankTexture(textures, textureId, 1000, 1000);
	Application::getInstance().sceneManager.trailTexture = CreateBlankTexture(textures, textureId, 3000, 3000);
	Application::getInstance().sceneManager.fieldOfViewTexture = CreateBlankTexture(textures, textureId, 1000, 1000);

	LoadTexture(textures, "Resources/img/HiddenSign.png", textureId);
	LoadTexture(textures, "Resources/img/DetectedSign.jpeg", textureId);

}

void ResourcesManager::AddShaders()
{
	unsigned int ShaderProgram = CreateShaderProgram("Resources/Shaders/TerrainFragShader.frag", "Resources/Shaders/TerrainVertexShader.vert");
	basicShaderProgram = ShaderProgram;
	shaders.push_back(ShaderProgram);

	ShaderProgram = CreateShaderProgram("Resources/Shaders/PlayerFragShader.frag", "Resources/Shaders/PlayerVertexShader.vert");
	shaders.push_back(ShaderProgram);

	ShaderProgram = CreateShaderProgram("Resources/Shaders/SkyBoxFrag.frag", "Resources/Shaders/SkyBoxVert.vert");
	shaders.push_back(ShaderProgram);

	ShaderProgram = CreateShaderProgram("Resources/Shaders/BrickFragShader.frag", "Resources/Shaders/TerrainVertexShader.vert");
	shaders.push_back(ShaderProgram);

	ShaderProgram = CreateGeoShaderProgram("Resources/Shaders/EnemyFragShader.frag", "Resources/Shaders/EnemyVertexShader.vert", "Resources/Shaders/ExplodeEnemy.geo");
	shaders.push_back(ShaderProgram);
}

int ResourcesManager::Init()
{
	AddTextures();
	AddModels();
	AddShaders();
	
	return 0;
}
void ResourcesManager::Update()
{
}

Model ResourcesManager::GetModel(int iD)
{

	for (Model model : models)
	{
		if (model.iD == iD)
			return model;
	}
}
Texture& ResourcesManager::GetTexture(int iD)
{
	for (Texture texture : textures)
	{
		if (texture.textureiD == iD)
			return texture;
	}
}

unsigned int ResourcesManager::GetShader(int iD)
{
	for (unsigned int shader : shaders)
	{
		if (shader == iD)
			return shader;
	}
}

void ULinkShader(GLuint program)
{
	glLinkProgram(program);

	GLint status;
	glGetProgramiv(program, GL_LINK_STATUS, &status);
	if (status == GL_FALSE)
	{
		GLint logLen;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLen);
		std::vector< char >log(logLen);
		GLsizei written;
		glGetProgramInfoLog(program, logLen, &written, log.data());
		std::cout << "link error:" << std::endl << log.data() << std::endl;
	}
}
unsigned int ResourcesManager::CreateShaderProgram(const char* fragmentShaderPath, const char* vertexShaderPath)
{
	Shader fragmentShader = Shader(fragmentShaderPath);
	Shader vertexShader = Shader(vertexShaderPath);

	const char* vertexShaderSource = vertexShader.shaderS.c_str();
	const char* fragmentShaderSource = fragmentShader.shaderS.c_str();

	vertexShader.shaderId = glCreateShader(GL_VERTEX_SHADER);
	fragmentShader.shaderId = glCreateShader(GL_FRAGMENT_SHADER);
	unsigned int shaderProgram = glCreateProgram();

	//Compiling
	glShaderSource(vertexShader.shaderId, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader.shaderId);
	glShaderSource(fragmentShader.shaderId, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader.shaderId);

	//attach
	glAttachShader(shaderProgram, vertexShader.shaderId);
	glAttachShader(shaderProgram, fragmentShader.shaderId);
	glLinkProgram(shaderProgram);

	ULinkShader(shaderProgram);

	fragmentShader.shaderDebug();
	vertexShader.shaderDebug();


	return shaderProgram;
}

unsigned int ResourcesManager::CreateGeoShaderProgram(const char* fragmentShaderPath, const char* vertexShaderPath, const char* geoShaderPath)
{
	Shader fragmentShader = Shader(fragmentShaderPath);
	Shader geoShader = Shader(geoShaderPath);
	Shader vertexShader = Shader(vertexShaderPath);

	const char* vertexShaderSource = vertexShader.shaderS.c_str();
	const char* fragmentShaderSource = fragmentShader.shaderS.c_str();
	const char* geoShaderSource  = geoShader.shaderS.c_str();

	vertexShader.shaderId = glCreateShader(GL_VERTEX_SHADER);
	fragmentShader.shaderId = glCreateShader(GL_FRAGMENT_SHADER);
	geoShader.shaderId = glCreateShader(GL_GEOMETRY_SHADER);
	unsigned int shaderProgram = glCreateProgram();

	//Compiling
	glShaderSource(vertexShader.shaderId, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader.shaderId);
	glShaderSource(fragmentShader.shaderId, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader.shaderId);
	glShaderSource(geoShader.shaderId, 1, &geoShaderSource, NULL);
	glCompileShader(geoShader.shaderId);

	//attach
	glAttachShader(shaderProgram, vertexShader.shaderId);
	glAttachShader(shaderProgram, geoShader.shaderId);
	glAttachShader(shaderProgram, fragmentShader.shaderId);
	glLinkProgram(shaderProgram);

	ULinkShader(shaderProgram);

	fragmentShader.shaderDebug();
	vertexShader.shaderDebug();


	return shaderProgram;
}
