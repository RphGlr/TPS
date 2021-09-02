#include <typeinfo>
#include "GameObject.h"

void GameObject::SetActive(bool activated)
{
	isEnabled = activated;
}

GameObject::GameObject(float3 pos, float3 rot)
{
	transform.position = pos;
	transform.rotation = rot;
	transform.scale = { 1,1,1 };
}

GameObject::GameObject(Transform t)
{
	transform = t;
}

GameObject::GameObject(Transform t, std::string name)
{
	transform = t;
	this->name = name;
}

