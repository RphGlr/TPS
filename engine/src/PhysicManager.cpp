#include "Application.h"
#include "Enemy.h"
#include "Player.h"

void PhysicManager::Init()
{
	deltaTime = &Application::getInstance().renderer.deltaTime;
}
void PhysicManager::Update(float deltaTime)
{
	if (Application::getInstance().gameManager.getState() != GameState::GAME)
		return;

	HandleCollisionWithTexture(deltaTime);

	for (std::shared_ptr<Enemy> enemy : enemies)
	{
		enemy.get()->Update(deltaTime);
	}

	UpdateFOVTexture();

}

void PhysicManager::ResetEnemies()
{
	for (std::shared_ptr<Enemy> enemy : enemies)
	{
		enemy.get()->cgameObject.isEnabled = true;
		enemy.get()->isDead = false;
	}
}

void StayInsideTheMap(GameObject& go)
{
	int mapWidth = Application::getInstance().sceneManager.renderTexture->width;
	int mapHeight = Application::getInstance().sceneManager.renderTexture->height;

	if (go.transform.position.x < 0)
		go.transform.position.x = 0;
	if (go.transform.position.x > mapWidth)
		go.transform.position.x = mapWidth;
	if (go.transform.position.z < 0)
		go.transform.position.z = 0;
	if (go.transform.position.z > mapHeight)
		go.transform.position.z = mapHeight;
}

void PhysicManager::HandleCollisionWithTexture(float deltaTime)
{
	Application& application = Application::getInstance();
	int index, value;

	//sample Collision texture 
	Texture* collisionTexture = application.sceneManager.renderTexture;
	int size = collisionTexture->width * collisionTexture->height;

	GameObject& playerGameObject = player.get()->cgameObject;

	player->Update(deltaTime);

	if (player->direction == float3{ 0,0,0 }) return;


	float3 deltaPosition = player->direction * player->speed * deltaTime;
	float3 nextPosition = playerGameObject.transform.position + deltaPosition;

	float3 orientedDirection = player->direction;

	index = (int)nextPosition.x * collisionTexture->width + (int)nextPosition.z;
	if (index > size || index < 0) return;

	value = collisionTexture->data[index];

	if (value != 255)
	{
		//offset pos off radius value in the player direction and check again

		float3 offsetPosition = nextPosition + player->direction * player->sphereColliderRadius;
		index = (int)offsetPosition.x * collisionTexture->width + (int)offsetPosition.z;

		value = collisionTexture->data[index];

		if (value != 255)
		{
			player->Move(deltaTime);
			StayInsideTheMap(playerGameObject);
		}

	}

}

void DrawCross(Texture* t , float3 pos, int value)
{
	int index;

	for (int i = -1; i <= 1; i++)
	{
		for (int j = -1; j <= 1; j++)
		{
			index = ((int)pos.x + i) * t->width + ((int)pos.z + j);
			t->data[index] = value;
		}
	}

}

void PhysicManager::UpdateFOVTexture()
{
	Texture* fovTexture = Application::getInstance().sceneManager.fieldOfViewTexture;
	Texture* CollisionTexture = Application::getInstance().sceneManager.renderTexture;

	int size = fovTexture->width * fovTexture->height;

	//clean fovTexture
	
	for (int i = 0; i < size; i++)
	{
		fovTexture->data[i] = 0;
	}

	int index;

	//Write enemy light view

	for (std::shared_ptr<Enemy> e : enemies)
	{
		if (e.get()->isDead) continue;

		Enemy* enemy = e.get();
		float3 enemyPos = enemy->cgameObject.transform.position;
		float distToPlayer = Distance(enemyPos, Application::getInstance().sceneManager.player->transform.position);
		if (distToPlayer > 150) continue;

		float3 enemyDir = enemy->direction;

		float3 tempPosition = enemyPos;

		float2 orientedDir = rotateByAngle({enemyDir.x, enemyDir.z}, -30);

		for (int i = 0; i < 60; i++)
		{
			tempPosition = enemyPos;

			orientedDir = rotateByAngle(orientedDir, 1);

			for (int j = 0; j < (int)e.get()->viewRange; j++)
			{
				tempPosition.x += orientedDir.x;
				tempPosition.z += orientedDir.y;

				if (tempPosition.x > CollisionTexture->width || tempPosition.x < 0 ||
					tempPosition.z > CollisionTexture->height || tempPosition.z < 0) continue;

				index = (int)tempPosition.x * CollisionTexture->width + (int)tempPosition.z;

				if (CollisionTexture->data[index] == 255) break;

				DrawCross(fovTexture, tempPosition,255);
			}

		}

	}

	glBindTexture(GL_TEXTURE_2D, fovTexture->textureGLLocation);

	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, fovTexture->width, fovTexture->height, GL_RED, GL_UNSIGNED_BYTE, fovTexture->data);

	glBindTexture(GL_TEXTURE_2D, 0);

}
