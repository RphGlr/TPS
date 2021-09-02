#include "Enemy.h"
#include "Application.h"
#include "Player.h"


Enemy::Enemy(GameObject& gameObject) : Enemy(gameObject, std::shared_ptr<Enemy>(this))
{
}

Enemy::Enemy(GameObject& gameObject, std::shared_ptr<Enemy> ptr) : Component(gameObject, ptr)
{
	Application::getInstance().physicManager.enemies.push_back(ptr);
}

void Enemy::Init()
{
	GetPatrolPath();
	cgameObject.transform.position = startPatrolPos;
	timeLocation = glGetUniformLocation(cgameObject.GetComponent<RenderingResources>().get()->material.shader.shaderId, "_time");
	player = Application::getInstance().sceneManager.player->GetComponent<Player>();
}

void Enemy::Update(float deltaTime)
{
	if (isDead)
		return;

	//if the enemy collide with player
	float3 playerPosition = player->cgameObject.transform.position;
	float distance = Distance(cgameObject.transform.position, playerPosition);

	if (distance < (sphereColliderRadius + player.get()->sphereColliderRadius) * 0.6)
		Application::getInstance().gameManager.setState(GameState::GAME_OVER);


	HandleState(deltaTime);
}

void Enemy::Move(float deltaTime)
{
	//Move enemy object
	float3 value = direction * speed * deltaTime;
	cgameObject.transform.position = cgameObject.transform.position + value;
}

void Enemy::CheckCollisions(float deltaTime)
{
	Texture* collisionTexture = Application::getInstance().sceneManager.renderTexture;
	
	float3 value = direction * speed * deltaTime;
	float3 nextPosition = cgameObject.transform.position + value;

	int index = (int)nextPosition.x * collisionTexture->width + (int)nextPosition.z;

	if (collisionTexture->data[index] > 128)
	{
		state = enemyState::BackToPatrol;
	}
}

void Enemy::GetPatrolPath()
{

	Application& application = Application::getInstance();

	Texture* collisionTexture = application.sceneManager.renderTexture;
	int size = collisionTexture->width * collisionTexture->height;
	int index, value;
	int patrolLength = 100;

	//get random starting pos inside map
	startPatrolPos = float3{ (float)(rand() % collisionTexture->width), 7 ,  (float)(rand() % collisionTexture->height) };

	index = (int)startPatrolPos.x * collisionTexture->width + (int)startPatrolPos.z;
	value = collisionTexture->data[index];


	//find right spot (not inside wall for example)
	while (value == 255 || startPatrolPos.x < 30 || startPatrolPos.z < 30)
	{
		startPatrolPos = float3{ (float)(rand() % collisionTexture->width), 7 ,  (float)(rand() % collisionTexture->height) };

		index = (int)startPatrolPos.x * collisionTexture->width + (int)startPatrolPos.z;
		value = collisionTexture->data[index];
	}

	float3 currentPos = startPatrolPos;

	//search for a good path direction
	float2 orientedVector = { 1,0 };

	int bestLength = -1;


	for (int i = 0; i < 180; i++)
	{
		float3 currentPos = startPatrolPos;
		orientedVector = rotateByAngle(orientedVector, 2);

		//check collision map in a straight line in orientedVector direction
		for (int j = 0; j <= patrolLength; j++)
		{
			currentPos.x += orientedVector.x;
			currentPos.z += orientedVector.y;

			index = (int)currentPos.x * collisionTexture->width + (int)currentPos.z;

			if (index <  0 || index > size)
				break;

			value = collisionTexture->data[index];
			if (value == 255)
			{
				if (j > bestLength && j < patrolLength)
				{
					bestLength = j;
					endPatrolPos = currentPos;
				}
				break;
			}
		}
	}

	//offset points of radius
	direction = Normalize(endPatrolPos - startPatrolPos);

	startPatrolPos += direction * sphereColliderRadius;
	endPatrolPos -= direction * sphereColliderRadius;

}

void Enemy::HandleState(float deltaTime)
{
	switch (state)
	{
	case enemyState::Patrol:
		PatrolState(deltaTime);
		break;
	case enemyState::BackToPatrol:
		BackToPatrolState(deltaTime);
		break;
	case enemyState::Attack:
		AttackState(deltaTime);
		break;
	default:
		break;
	}

}

//Idle state / patrolling
void Enemy::PatrolState(float deltaTime)
{
	float distance = 0;
	float3 tempDir;

	//Is Enemy going in the right direction ? 
	tempDir = Normalize(endPatrolPos - cgameObject.transform.position);
	float dotProduct = DotProduct(tempDir, direction);

	//going the wrong way / went further than the point it needed to reach
	if (dotProduct < 0)
	{
		float3 temp = startPatrolPos;
		startPatrolPos = endPatrolPos;
		endPatrolPos = temp;

		direction = Normalize(endPatrolPos - startPatrolPos);
	}

	Move(deltaTime);

	//check if player is in front of enemy and too close
	float3 playerPosition = player->cgameObject.transform.position;

	Texture* fovTexture = Application::getInstance().sceneManager.fieldOfViewTexture;
	int index = (int)playerPosition.x * fovTexture->width + (int)playerPosition.z;

	if (fovTexture->data[index] < 128)
		return;

	distance = Distance(cgameObject.transform.position, playerPosition);
	tempDir = Normalize(playerPosition - cgameObject.transform.position);
	dotProduct = DotProduct(tempDir, direction);

	if (dotProduct > 0.7)
	{
		if (distance < 10)
		{
			//Instant Death
			Application::getInstance().gameManager.setState(GameState::GAME_OVER);
		}
		else if (distance > 10 && distance < viewRange)
		{
			state = enemyState::Attack;
			quitPatrolPos = cgameObject.transform.position;
		}
	}
}

//Go Back to Where he left off the patrol
void Enemy::BackToPatrolState(float deltaTime)
{
	float distance = Distance(cgameObject.transform.position, quitPatrolPos);
	direction = Normalize(quitPatrolPos - cgameObject.transform.position);
	
	if (distance > 7)
	{
		Move(deltaTime);
	}
	else
	{
		cgameObject.transform.position = quitPatrolPos;
		state = enemyState::Patrol;
	}
}

//Chase Player
void Enemy::AttackState(float deltaTime)
{
	float3 playerPosition = player->cgameObject.transform.position;
	float distance = Distance(cgameObject.transform.position, playerPosition);

	if (distance > 10 && distance < viewRange)
	{
		direction = Normalize(playerPosition - cgameObject.transform.position);
		direction.y = 0;
		CheckCollisions(deltaTime);
		Move(deltaTime);
	}
	else
	{
		state = enemyState::BackToPatrol;
	}

}

