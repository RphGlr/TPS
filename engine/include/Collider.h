#pragma once
#include <vector>
#include <glad/glad.h>
#include "Types.h"
#include "Component.h"

class BoxCollider;
class SphereCollider;

struct Collision
{
	bool isColliding = false;
	float3 intersectionPoint = { 0 };
};

class Collider : public Component
{
public:
	Collider(GameObject& gameObject);
	Collider(GameObject& gameObject, std::shared_ptr<Collider> ptr);
	float3 localPos = { 0 };

	Collision SphereBoxCollision(SphereCollider s, BoxCollider b);
	Collision SphereSphereCollision(SphereCollider s1, SphereCollider s2);
	Collision BoxBoxCollision(BoxCollider b1, BoxCollider b2, float3& p);

	virtual void Draw(mat4x4& translate, mat4x4& rotate, mat4x4& scale, mat4x4& model, const GLint& modelLocation, void* resourcesManager) {};
};

class BoxCollider : public Collider
{
public:
	BoxCollider(GameObject&, sCube);
	BoxCollider(GameObject&, float3);

	sCube cube;

	void Draw(mat4x4& translate, mat4x4& rotate, mat4x4& scale, mat4x4& model, const GLint& modelLocation, void* resourcesManager) override;
};
class SphereCollider : public Collider
{
public:
	SphereCollider(GameObject&, sSphere);
	SphereCollider(GameObject&, float radius);

	sSphere sphere;
	void Draw(mat4x4& translate, mat4x4& rotate, mat4x4& scale, mat4x4& model, const GLint& modelLocation, void* resourcesManager) override;

};

using namespace Maths;

inline bool operator==(SphereCollider a, SphereCollider b)
{
	bool samePos = a.localPos == b.localPos;
	bool sameSphere = a.sphere == b.sphere;
	bool sameObject = &a.cgameObject == &b.cgameObject;

	return samePos && sameSphere && sameObject;
}