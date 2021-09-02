
#include "Component.h"

Component::Component(GameObject& gameObject, std::shared_ptr<Component> child)
	: cgameObject(gameObject)
{
	cgameObject.components.push_back(child);
}
