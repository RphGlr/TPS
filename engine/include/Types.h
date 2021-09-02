#pragma once
#include <vector>
#include "Maths.h"

using namespace Maths;

struct boundingBox
{
	float minX, maxX;
	float minY, maxY;
};

struct sSphere
{
	float radius = 0;
};

inline bool operator==(sSphere a, sSphere b)
{
	return a.radius == b.radius;
}

struct sCube
{
	float width = 0, height = 0, length = 0;
};

typedef struct rdrVertex
{
	float x, y, z;    // Pos
	float nx, ny, nz; // Normal
	float u, v;       // Texture coordinates
} rdrVertex;

typedef struct triangle
{
	rdrVertex vertices[3];
} face;

struct Transform
{
	float3 position = { 0,0,0 };
	float3 rotation = { 0,0,0 };
	float3 scale = { 1.0f,1.0f,1.0f };

	Transform AddTransform(Transform t1, Transform t2)
	{
		Transform res;
		res.position = t1.position + t2.position;
		res.rotation = t1.rotation + t2.rotation;
		res.scale = { t1.scale.x * t2.scale.x, t1.scale.y * t2.scale.y, t1.scale.z * t2.scale.z };

		return res;
	}
};

class Light
{
public:
	Light() = default;
	Light(float4 pos) : position{ pos } {}

	float4 position = { 0.f,10.f,0.f,0.f }; // 3D pos + Light intensity (value)
	float4 color = { 1.f,1.f,1.f,1.f };
	float4 values = { 1.f,1.f,1.f,1.f }; //Light Strength


	float4 tampon2 = { 1.f,1.f,1.f,1.f };

	float specularStrength = 0.5f;
};

class Mesh
{
public:
	Mesh() = default;

	std::vector<triangle> triangles;
};

