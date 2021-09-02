#pragma once
#include "Component.h"

enum class enemyState
{
	Patrol,
	BackToPatrol,
	Attack
};

class Player;
class Enemy : public Component
{
public:
	Enemy(GameObject& gameObject);
	Enemy(GameObject& gameObject, std::shared_ptr<Enemy> ptr);

	void Init();
	void Update(float deltaTime);
	void Move(float deltaTime);
	void CheckCollisions(float deltaTime);


	void GetPatrolPath();

	void HandleState(float deltaTime);

	void PatrolState(float deltaTime);
	void BackToPatrolState(float deltaTime);
	void AttackState(float deltaTime);


	std::shared_ptr<Player> player;

	float3 startPatrolPos;
	float3 endPatrolPos;
	float3 quitPatrolPos;

	float3 direction;
	float sphereColliderRadius = 10.0f;

	float speed = 20;
	float viewRange = 50;
	bool isDead = false;

	int timeLocation;

private :
	enemyState state = enemyState::Patrol;
};