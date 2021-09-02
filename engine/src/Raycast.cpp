#include "Raycast.h"
#include "Collider.h"

bool solveQuadratic(const float& a, const float& b, const float& c, float& x0, float& x1)
{
	float discr = b * b - 4 * a * c;
	if (discr < 0) return false;
	else if (discr == 0) x0 = x1 = -0.5 * b / a;
	else {
		float q = (b > 0) ?
			-0.5 * (b + sqrt(discr)) :
			-0.5 * (b - sqrt(discr));
		x0 = q / a;
		x1 = c / q;
	}
	if (x0 > x1) std::swap(x0, x1);

	return true;
}

bool RaycastIntersectGeometric(Raycast& ray, std::shared_ptr<SphereCollider>& sc, float3& hitPosition, float3& hitNormal)
{
	// geometric solution

	float t0, t1; // solutions for t if the ray intersects 

	float3 L = sc.get()->localPos - ray.origin;
	float tca = DotProduct(L, ray.dir);
	// if (tca < 0) return false;
	float d2 = DotProduct(L, L) - tca * tca;
	if (d2 > sc.get()->sphere.radius) return false;
	float thc = sqrt(sc.get()->sphere.radius - d2);
	t0 = tca - thc;
	t1 = tca + thc;

	if (t0 > t1) std::swap(t0, t1);

	if (t0 < 0) {
		t0 = t1; // if t0 is negative, let's use t1 instead 
		if (t0 < 0) return false; // both t0 and t1 are negative 
	}

	float t = t0;

	hitPosition = ray.origin + ray.dir * t;
	hitNormal = Normalize(hitPosition - sc.get()->localPos);

	return true;
}
bool RaycastIntersect(Raycast& ray, std::shared_ptr<SphereCollider>& sc, float3& hitPosition, float3& hitNormal)
{

	// analytic solution
	float t0, t1; // solutions for t if the ray intersects 

	float3 L = ray.origin - sc.get()->localPos;
	float a = DotProduct(ray.dir, ray.dir);
	float b = 2 * DotProduct(ray.dir, L);
	float c = DotProduct(L, L) - sc.get()->sphere.radius;
	if (!solveQuadratic(a, b, c, t0, t1)) return false;

	if (t0 > t1) std::swap(t0, t1);

	if (t0 < 0) {
		t0 = t1; // if t0 is negative, let's use t1 instead 
		if (t0 < 0) return false; // both t0 and t1 are negative 
	}

	float t = t0;

	hitPosition = ray.origin + ray.dir * t;
	hitNormal = Normalize(hitPosition - sc.get()->localPos);

	return true;
}

bool IntersectionSegmentSphere(const float3& a, const float3& b, std::shared_ptr<SphereCollider> sphereCollider, float3& interPt, float3& interNormal)
{
	sSphere sphere = sphereCollider.get()->sphere;

	float3 d = (b - a) / Norm(b - a);
	float3 m = a - (sphereCollider.get()->localPos + sphereCollider.get()->cgameObject.transform.position);

	float c = DotProduct(m, m) - sphere.radius * sphere.radius;
	float b_ = DotProduct(m, d);

	if (c > 0 && b_ > 0)
		return false;

	float delta = b_ * b_ - c;

	if (delta < 0)
		return false;

	float t = -b_ - sqrt(delta);

	if (t < 0.f)
		t = 0;

	interPt = a + d * t;
	interNormal = (interPt - (sphereCollider.get()->localPos + sphereCollider.get()->cgameObject.transform.position)) / sphere.radius;

	return true;
}