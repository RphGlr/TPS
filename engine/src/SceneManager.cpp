#include <random>

#include "Application.h"
#include "Enemy.h"
#include "Camera.h"
#include "Player.h"

void SceneManager::Update()
{
	if (sceneiD != current_scene->iD)
	{
		ChangeScene(sceneiD);
	}
}

GameObject& AddEntityToScene(Scene& current_scene, Transform t, std::string name)
{
	current_scene.hierarchy.push_back(GameObject(t, name));
	return current_scene.hierarchy.back();
}
Scene& AddNewScene(std::deque<Scene>& scenes, int& iD)
{
	scenes.push_back(Scene());
	Scene& ref = scenes.back();
	ref.iD = iD;
	iD++;

	return ref;
}

Enemy* AddEnemyToScene(Scene& scene, int enumTexture, float colliderRadius)
{
	Model m = Application::getInstance().resourcesManager.GetModel((int)Sphere);
	m.texture = Application::getInstance().resourcesManager.GetTexture(enumTexture);

	Material mat;
	mat.shader.shaderId = Application::getInstance().resourcesManager.GetShader(EnemyShaderProgram);

	GameObject& entity = AddEntityToScene(scene, Transform(), "Enemy");
	entity.isEnabled = true;

	RenderingResources* rs = entity.AddComponent<RenderingResources>(m, mat);
	Enemy* enemyComponent = entity.AddComponent<Enemy>();
	enemyComponent->Init();

	rs->transform.scale = rs->transform.scale * colliderRadius;

	return enemyComponent;
}

void SceneManager::Init()
{
	int sceneId = 0;
	srand(time(NULL));

	//Scene 0 : Going to be the Game Scene
	{
		Scene& scene = AddNewScene(scenes, sceneId);
		current_scene = &scene;
		{
			GameObject& entity = AddEntityToScene(scene, Transform(), "Player");
			entity.isEnabled = true;

			entity.AddComponent<Camera>();

			entity.transform.position = { 0,1,0 };

			player = &entity;
		}
	}
}

void SceneManager::LateInit(void* resourcesManagerPtr)
{
	ResourcesManager* resourcesManager = (ResourcesManager*)resourcesManagerPtr;
	Application& application = Application::getInstance();

	int sceneId = 1;

	//Scene 0 : Going to be the Game Scene
	{
		Scene& scene = scenes.back();


		//Entity 1
		Model m = resourcesManager->GetModel((int)Sphere);
		Material mat;
		mat.shader.shaderId = application.resourcesManager.GetShader(PlayerShaderProgram);

		{
			GameObject& entity = scene.hierarchy.back(); //Player Scene 1
			entity.isEnabled = true;
			float colliderRadius = 1.0f;

			entity.AddComponent<Player>(entity.GetComponent<Camera>());
			RenderingResources* rs = entity.AddComponent<RenderingResources>(m, mat);
			rs->transform.scale = { colliderRadius / 2, colliderRadius / 2 , colliderRadius / 2 };

			std::shared_ptr<Player> player = entity.GetComponent<Player>();
			player->sphereColliderRadius = colliderRadius;
			player->Init();

			Application::getInstance().physicManager.player = player;

			entity.transform.position = { 0,1,0 };

		}

		InitObstacles(scene);

		Enemy* e = AddEnemyToScene(scene, BlankTexture,1);
		enemyTransform = &e->cgameObject.transform;
		
		for (int i = 0; i < 100; i++)
		{
			AddEnemyToScene(scene, BlankTexture, 4);
		}

		// SkyBOX


		m = resourcesManager->GetModel((int)Sphere);
		{
			mat.shader.shaderId = resourcesManager->GetShader(SkyBoxShaderProgram);

			GameObject& entity = AddEntityToScene(scene, Transform(), "SkyBox");
			//entity added to scene, we can add/modify components bellow
			//---------------------------BELLOW-------------------------
			entity.isEnabled = true;
			entity.transform.scale = { 500,500,500 };
			entity.transform.position = { 500,0,500 };

			groundRotation = &entity.transform.rotation;
			groundScale = &entity.transform.scale;

			m.texture = resourcesManager->GetTexture(CollisionTexture);
			entity.AddComponent<RenderingResources>(m, mat);

		}

		mat.shader.shaderId = application.resourcesManager.GetShader(TerrainShaderProgram);

		//Entity Ground
		m = resourcesManager->GetModel((int)Plane);
		{
			GameObject& entity = AddEntityToScene(scene, Transform(), "Ground Scene 1");
			//entity added to scene, we can add/modify components bellow
			//---------------------------BELLOW-------------------------
			entity.isEnabled = true;
			entity.transform.scale = { 500,1,500 };
			entity.transform.position = { 500,0,500 };
			entity.transform.rotation = { 0,180 * 0.0174533,0 };

			groundRotation = &entity.transform.rotation;
			groundScale = &entity.transform.scale;

			m.texture = resourcesManager->GetTexture(CollisionTexture);
			entity.AddComponent<RenderingResources>(m, mat);

		}

		

	}
	//Scene 1 : Menu Scene
	{
		Scene& scene = AddNewScene(scenes, sceneId);
		current_scene = &scene;
	}
}
void SceneManager::ChangeScene(int iD)
{
	if (iD == current_scene->iD)
		return;

	for (GameObject& go : current_scene->hierarchy)
	{
		go.isEnabled = false;
	}

	current_scene = &scenes[iD];

	for (GameObject& go : current_scene->hierarchy)
	{
		go.isEnabled = true;
	}
}

void SceneManager::InitObstacles(Scene& current_scene)
{
	Application& application = Application::getInstance();
	ResourcesManager& resourcesManager = application.resourcesManager;

	Material mat;
	mat.shader.shaderId = resourcesManager.GetShader(BrickShaderProgram);

	Model m = resourcesManager.GetModel((int)Cube);;
	m.texture = resourcesManager.GetTexture(BlankTexture);

	Texture* collisionTexture = Application::getInstance().sceneManager.renderTexture;
	Texture* trailTexture = Application::getInstance().sceneManager.trailTexture;


	for (size_t i = 1; i < 20; i++)
	{
		for (size_t j = 1; j < 20; j++)
		{
			float xScale = rand() % 10 + 10; //  10 to 19
			float yScale = rand() % 30 + 10; //10 to 39
			float zScale = rand() % 10 + 10; // 10 to 19

			{
				GameObject& entity = AddEntityToScene(current_scene, Transform(), "Cube");
				//entity added to scene, we can add/modify components bellow
				//---------------------------BELLOW-------------------------
				entity.isEnabled = true;
				entity.transform.scale = { xScale,yScale,zScale };
				entity.transform.position = { 50.0f * i, yScale,50.0f * j };

				entity.AddComponent<RenderingResources>(m, mat);

				//Draw Obstacles on CollisionTexture

				const float sizeCollisionTex = collisionTexture->width * collisionTexture->height;
				const float sizeTrailTex = trailTexture->width * trailTexture->height;


				for (size_t x = 0; x < xScale * 2; x++)
				{
					for (size_t z = 0; z < zScale * 2; z++)
					{
						int xIndex = ((int)50.f * i + x - xScale);
						int zIndex = (int)50.f * j + z - zScale;

						int indexCollisionTex = xIndex * collisionTexture->width + zIndex;

						//if (index > size - textureWidth || index < textureWidth) return;
						if (indexCollisionTex < sizeCollisionTex - collisionTexture->width && indexCollisionTex > collisionTexture->width)
						{
							collisionTexture->data[indexCollisionTex] = 255;
						}

					}
				}

				glBindTexture(GL_TEXTURE_2D, collisionTexture->textureGLLocation);
				glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, collisionTexture->width, collisionTexture->height, GL_RED, GL_UNSIGNED_BYTE, collisionTexture->data);

				glBindTexture(GL_TEXTURE_2D, trailTexture->textureGLLocation);
				glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, trailTexture->width, trailTexture->height, GL_RED, GL_UNSIGNED_BYTE, trailTexture->data);

				glBindTexture(GL_TEXTURE_2D, 0);

			}
		}

	}

}

void Scene::Draw(mat4x4& translate, mat4x4& rotate, mat4x4& scale, mat4x4& model, int wireFrame)
{
	Application& application = Application::getInstance();

	for (GameObject entity : hierarchy)
	{
		if (!entity.isEnabled)
			continue;

		if (wireFrame == 0) //no wireframe
		{
			std::shared_ptr<RenderingResources> rs = entity.GetComponent<RenderingResources>();
			if (rs != NULL)
				rs.get()->Draw(translate, rotate, scale, model);
		}

	}

}
