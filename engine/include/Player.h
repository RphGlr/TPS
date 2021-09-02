#pragma once
#include "Component.h"

class Camera;
class Player : public Component
{
protected:
	struct PlayerInputs
	{
		bool moveForward = false;
		bool moveBackward = false;
		bool strafeLeft = false;
		bool strafeRight = false;
		bool moveUp = false;
		bool moveDown = false;
	};

public :
	Player(GameObject& gameObject, std::shared_ptr<Camera> camera);
	Player(GameObject& gameObject, std::shared_ptr<Player> ptr);

	void Init();
	void Update(float deltaTime);
	void CheckFovDetection(float deltaTime);

	void Move(float deltaTime);
	void Kill();
	void LinkInputs();

	void Reset();

	bool invincibility = false;

	int kills = 0;
	int score = 0;
	int health = 3;

	float burningTimer = 0;
	float sphereColliderRadius;
	float3 direction = { 0,0,0 };
	float speed = 50;
	float killRange = 10;
	bool isDead = false;

private :
	PlayerInputs pInputs;
	float3 startingPosition = { 0,0,0 };

	std::shared_ptr<Camera> camera;

	void HandleInputs(float& forwardMove, float& strafeMove);
};
