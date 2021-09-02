#pragma once
#include <vector>
#include <memory>
#include "Types.h"


class SphereCollider;
class BoxCollider;

struct Raycast
{
	float3 origin;
	float3 dir;
};

bool RaycastIntersectGeometric(Raycast& ray, std::shared_ptr<SphereCollider>& sc, float3& hitPosition, float3& hitNormal);
bool RaycastIntersect(Raycast& ray, std::shared_ptr<SphereCollider>& sc, float3& hitPosition, float3& hitNormal);
bool IntersectionSegmentSphere(const float3& a, const float3& b, std::shared_ptr<SphereCollider> sphereCollider, float3& interPt, float3& interNormal);


