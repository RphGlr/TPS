#include "Camera.h"
#include "Input.h"
#include "Maths.h"
#include "Application.h"
#include "Raycast.h"
#include "Player.h"

using namespace Core;

Player::Player(GameObject& gameObject, std::shared_ptr<Camera> cam) : Player(gameObject, std::shared_ptr<Player>(this))
{
	camera = cam;
}

Player::Player(GameObject& gameObject, std::shared_ptr<Player> ptr) : Component(gameObject, ptr)
{
	Application::getInstance().physicManager.player = ptr;
}

void HandleInputs()
{
}

void Player::LinkInputs()
{
	Input::instance()->actionsPressed.find(GLFW_KEY_W)->second = [&] { pInputs.moveForward = true; };
	Input::instance()->actionsPressed.find(GLFW_KEY_S)->second = [&] { pInputs.moveBackward = true; };
	Input::instance()->actionsPressed.find(GLFW_KEY_D)->second = [&] { pInputs.strafeRight = true; };
	Input::instance()->actionsPressed.find(GLFW_KEY_A)->second = [&] { pInputs.strafeLeft = true; };

	Input::instance()->actionsPressed.find(GLFW_KEY_SPACE)->second = [&] {Kill(); };

	Input::instance()->actionsReleased.find(GLFW_KEY_W)->second = [&] { pInputs.moveForward = false; };
	Input::instance()->actionsReleased.find(GLFW_KEY_S)->second = [&] { pInputs.moveBackward = false; };
	Input::instance()->actionsReleased.find(GLFW_KEY_D)->second = [&] { pInputs.strafeRight = false; };
	Input::instance()->actionsReleased.find(GLFW_KEY_A)->second = [&] { pInputs.strafeLeft = false; };

	//Input::instance()->mousePressed.find(GLFW_MOUSE_BUTTON_LEFT)->second = [&] { Shoot(); };
}

void Player::Reset()
{
	cgameObject.transform.position = startingPosition;
	isDead = false;
	burningTimer = 0;

	score = 0;
}

using namespace Maths;

void Player::Init()
{
	LinkInputs();
	startingPosition = cgameObject.transform.position;
}

void Player::Update(float deltaTime)
{
	if (isDead)
	{
		if (Application::getInstance().gameManager.getState() != GameState::GAME_OVER)
			Application::getInstance().gameManager.setState(GameState::GAME_OVER);

		return;
	}

	float forwardMovement, strafeMovement;

	HandleInputs(forwardMovement, strafeMovement);


	if (invincibility)
	{
		camera.get()->shakeAmount = 0;
		return;
	}

	CheckFovDetection(deltaTime);

	if (burningTimer > 0.1)
		isDead = true;

	health = 10 - (burningTimer * 100) > 0 ? 10 - (burningTimer * 100) : 0;

}

void Player::CheckFovDetection(float deltaTime)
{
	Texture* fovTexture = Application::getInstance().sceneManager.fieldOfViewTexture;

	int index = (int)cgameObject.transform.position.x * fovTexture->width + (int)cgameObject.transform.position.z;
	if (fovTexture->data[index] > 128)
	{
		burningTimer += deltaTime * .2;
		camera.get()->shakeAmount = burningTimer;
	}
	else
	{
		camera.get()->shakeAmount = burningTimer;
		burningTimer -= deltaTime * 2;

		if (burningTimer < 0) burningTimer = 0;
	}

}

void Player::Move(float deltaTime)
{
	float3 value = direction * speed * deltaTime;
	
	cgameObject.transform.rotation += direction;
	cgameObject.transform.position = cgameObject.transform.position + value;
}

void Player::Kill()
{
	if (Application::getInstance().gameManager.getState() != GameState::GAME)
		return;

	Enemy* closestEnemy = NULL;
	Application& application = Application::getInstance();

	for (std::shared_ptr<Enemy> e : application.physicManager.enemies)
	{
		if (closestEnemy == NULL)
		{
			closestEnemy = e.get();
			continue;
		}

		float3 ePos = e.get()->cgameObject.transform.position;

		if (Distance(cgameObject.transform.position, ePos) < Distance(cgameObject.transform.position, closestEnemy->cgameObject.transform.position))
		{
			if (DotProduct(direction, ePos - cgameObject.transform.position) > 0.3) //in front of player
				closestEnemy = e.get();
		}
	}


	float distance = Distance(cgameObject.transform.position, closestEnemy->cgameObject.transform.position);

	if (distance < killRange)
	{
		closestEnemy->isDead = true;
		closestEnemy->cgameObject.isEnabled = false;

		score++;
	}

	if (score == 25)
	{
		application.gameManager.setState(GameState::VICTORY);
	}

}

//Update forwardMovement, strafeMovement to get direction depending on the inputs
void Player::HandleInputs(float& forwardMovement, float& strafeMovement)
{
	float deltaTime = Application::getInstance().renderer.deltaTime;

	forwardMovement = 0.f;
	strafeMovement = 0.f;

	if (pInputs.moveForward)
		forwardMovement += speed * deltaTime;
	if (pInputs.moveBackward)
		forwardMovement -= speed * deltaTime;

	if (pInputs.strafeRight)
		strafeMovement += speed * deltaTime;
	if (pInputs.strafeLeft)
		strafeMovement -= speed * deltaTime;

	direction = { 0,0,0 };

	direction.x += Maths::sin(camera.get()->yaw) * forwardMovement;
	direction.z -= Maths::cos(camera.get()->yaw) * forwardMovement;
	direction.x += Maths::cos(camera.get()->yaw) * strafeMovement;
	direction.z += Maths::sin(camera.get()->yaw) * strafeMovement;

	direction = Normalize(direction);
}
