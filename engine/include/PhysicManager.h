#pragma once
#include <deque>

#include "Enemy.h"
#include "Player.h"

class PhysicManager
{
public :
	PhysicManager() = default;
	~PhysicManager() = default;

	std::shared_ptr<Player> player;

	std::deque<std::shared_ptr<Enemy>> enemies;

	
	double* deltaTime = nullptr;

	void Init();
	void Update(float deltaTime);

	void ResetEnemies();

private :

	void HandleCollisionWithTexture(float deltaTime);

	void UpdateFOVTexture();

};

