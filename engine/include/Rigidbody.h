#pragma once
#include <unordered_map>
#include "Component.h"

struct ephemeralForce
{
	float3 value = { 0,0,0 };
	float duration = 0;
	int iD = 0;
};

inline bool operator==(ephemeralForce a, ephemeralForce b)
{
	bool sameValue = a.value == b.value;
	bool sameDuration = a.duration == b.duration;
	bool sameiD = a.iD == b.iD;

	return sameValue && sameDuration && sameiD;
}

class Application;
class Rigidbody : public Component
{
public:
	Rigidbody(GameObject& gameObject);
	Rigidbody(GameObject& gameObject, float mass);
	Rigidbody(GameObject& gameObject,std::shared_ptr<Rigidbody> ptr);

	~Rigidbody(){}

	void Init();

	Application* application;

	float3 speed = { 0,0,0 };
	float3 acceleration = { 0,0,0 };
	float mass = 1;
	bool isGrounded = false;

	std::vector<float3> forces;
	std::vector<ephemeralForce> ephemeralForces; 
	
	void AddEphemeralForce(float3 force, float duration)
	{
		ephemeralForces.push_back(ephemeralForce{force,duration, (int)ephemeralForces.size()});
	}
	void UpdateEphemeralForces(float deltaTime);


	float3 ComputeNextPositionDelta(float deltaTime);
	float3 GetFriction(float coefficient);
	float3 GetGroundReaction(float3 totalForces);

	void UpdateMotion(float deltaTime);
	void UpdateAcceleration();
	void UpdatePosition(float deltaTime);
	void UpdateSpeed(float deltaTime);

private : 
	inline float3 GetSpeedFromAcceleration(float3 currentSpeed, float3 acceleration, float deltaTime)
	{
		return currentSpeed + acceleration * deltaTime;
	}
	inline float3 GetPositionFromSpeed(float3 currentPos, float3 currentSpeed, float deltaTime)
	{
		return currentPos + currentSpeed * deltaTime;
	}
};