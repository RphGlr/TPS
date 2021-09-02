#pragma once
#include <iostream>
#include <deque>
#include "Types.h"

class Component;

class GameObject
{
public :
	GameObject() = default;
	GameObject(Transform t);
	GameObject(Transform t, std::string name);
	GameObject(float3 pos, float3 rot);

	~GameObject() = default;

	std::string name;
	std::deque<std::shared_ptr<Component>> components;
	Transform transform;
	bool isStatic = false;
	bool isEnabled = false;

	void SetActive(bool activated);
	
	template <class T, typename B = std::enable_if_t<std::is_base_of<Component, T>::value>>
	std::shared_ptr<T> GetComponent()
	{
		std::shared_ptr<T> returnedComponentPtr;

		for (std::shared_ptr<Component> componentPtr : components)
		{
			auto tryComponent = std::dynamic_pointer_cast<T>(componentPtr);

			if (tryComponent)
			{
				returnedComponentPtr = tryComponent;
				return returnedComponentPtr;
			}
		}

		return NULL;
	}

	template <class T, typename ... Args, typename B = std::enable_if_t<std::is_base_of<Component, T>::value>>
	T* AddComponent(Args ... params)
	{
		return new T(*this, params...);
	}

	template <class T, typename B = std::enable_if_t<std::is_base_of<Component, T>::value>>
	void RemoveComponent(T* t)
	{
		int i = 0;
		for (std::shared_ptr<Component> componentPtr : components)
		{
			std::shared_ptr<T> tryComponent = std::dynamic_pointer_cast<T>(componentPtr);

			if (tryComponent && tryComponent.get() == t)
			{
				components.erase(components.begin() + i);
			}

			i++;
		}
	}

};
