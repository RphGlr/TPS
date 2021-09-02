#pragma once
#include <memory>
#include "GameObject.h"

class Component
{
public:
	GameObject& cgameObject;

	Component() = default;
	Component(GameObject& gameObject, std::shared_ptr<Component> child);

	virtual ~Component() = default;

	virtual void Start() {}
	virtual void Update() {}

};
