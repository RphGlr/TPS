#pragma once
#include <deque>
#include "GameObject.h"
#include "RenderingResources.h"

class Player;
class Scene
{
public:
	std::deque<GameObject> hierarchy;
	int iD;
	void Draw(mat4x4& translate, mat4x4& rotate, mat4x4& scale, mat4x4& model,int wireFrame);

};

class SceneManager
{
public:

	std::deque<Scene> scenes;
	Scene* current_scene;
	int sceneiD;

	void Init();
	void LateInit(void* resourcesManagerPtr);
	void Update();
	void ChangeScene(int iD);

	void InitObstacles(Scene& current_scene);

	//debug

	float3* groundRotation;
	float3* groundScale;

	Texture* renderTexture;
	Texture* trailTexture;
	Texture* fieldOfViewTexture;

	GameObject* player;
	Transform* enemyTransform;
};

