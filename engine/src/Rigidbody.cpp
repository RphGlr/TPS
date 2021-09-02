#include "..\include\Rigidbody.h"
#include "Application.h"
#include "Collider.h"


using namespace Maths;


Rigidbody::Rigidbody(GameObject& gameObject) : Rigidbody(gameObject, std::shared_ptr<Rigidbody>(this))
{
	Init();
}
Rigidbody::Rigidbody(GameObject& gameObject, float mass) : Rigidbody(gameObject, std::shared_ptr<Rigidbody>(this))
{
	Init();
	this->mass = mass;
}
Rigidbody::Rigidbody(GameObject& gameObject, std::shared_ptr<Rigidbody> ptr) : Component(gameObject, ptr)
{
	application = &Application::getInstance();
	application->physicManager.rigidBodies.push_back(ptr);
}

void Rigidbody::Init()
{
	float3 gravity = { 0,-10,0 };
	forces.push_back(gravity * 2);
}

float3 Rigidbody::ComputeNextPositionDelta(float deltaTime) //Get next position
{
	UpdateAcceleration();
	speed = GetSpeedFromAcceleration(speed, acceleration, deltaTime);

	float3 deltaPos = GetPositionFromSpeed(float3{ 0,0,0 }, speed, deltaTime);

	return deltaPos;
}
void Rigidbody::UpdateEphemeralForces(float deltaTime)
{
	for (ephemeralForce& eF : ephemeralForces)
	{
		eF.duration -= deltaTime;

		if (eF.duration <= 0)
		{
			ephemeralForces.erase(std::remove(ephemeralForces.begin(), ephemeralForces.end(), eF), ephemeralForces.end());
		}
	}
}

void Rigidbody::UpdateMotion(float deltaTime)
{
	UpdateAcceleration();
	UpdateSpeed(deltaTime);
	UpdatePosition(deltaTime);
}
void Rigidbody::UpdateAcceleration()
{
	float3 totalForce = { 0 };
	for (float3 force : forces)
	{
		totalForce = totalForce + force;
	}

	for (int i = 0; i < ephemeralForces.size(); i++)
	{
		totalForce = totalForce + ephemeralForces[i].value;
	}

	//Add Forces if on Ground
	std::shared_ptr<SphereCollider> collider = cgameObject.GetComponent<SphereCollider>();
	if (collider != nullptr)
	{
		bool colliderTouchingGround = (collider.get()->localPos.y + cgameObject.transform.position.y) - collider.get()->sphere.radius <= 0;

		if (colliderTouchingGround && !isGrounded)
		{
			isGrounded = true;
			speed.y = 0;
			cgameObject.transform.position.y = cgameObject.GetComponent<SphereCollider>().get()->sphere.radius;
		}
		else if (colliderTouchingGround)
		{
			totalForce = totalForce + GetFriction(0.5f);
			totalForce = totalForce + GetGroundReaction(totalForce);
			
			if (speed.y < 0)
				speed.y = 0;
		}
		else
			isGrounded = false;

	}
	else
	{
		if (cgameObject.transform.position.y <= 0)
		{
			cgameObject.transform.position.y = 0;

			if (speed.y < 0)
				speed.y = 0;
		}
	}
			

	acceleration = totalForce / mass;
}
void Rigidbody::UpdateSpeed(float deltaTime)
{
	float maxSpeedNorm = 200.0f;

	if (Norm(speed) > maxSpeedNorm)
	{
		speed = Normalize(speed) * maxSpeedNorm;
		return;
	}

	speed = GetSpeedFromAcceleration(speed, acceleration, deltaTime);
}

void Rigidbody::UpdatePosition(float deltaTime) //Move the object 
{
	float3 newPosition = GetPositionFromSpeed(cgameObject.transform.position, speed, deltaTime);

	if (Distance(newPosition, cgameObject.transform.position) > 50.0f)
		return;

	cgameObject.transform.position = GetPositionFromSpeed(cgameObject.transform.position, speed, deltaTime);
}

float3 Rigidbody::GetGroundReaction(float3 totalForces)
{
	if (totalForces.y > 0)
		return float3{ 0 };

	return float3{0,-totalForces.y,0};
}
float3 Rigidbody::GetFriction(float f)
{
	return float3{-speed.x * f, 0, -speed.z * f};
}
