#pragma once
#include <list>
#include <GLFW/glfw3.h>

#include "Types.h"
#include "Texture.h"

struct Model
{
public:
	Model() = default;
	Model(const char* objPath, Texture texture, float3 position, int iD);
	Model(Model m, Texture texture, float3 position, int iD);

	void PopulateTrianglesArray(const char* path);
	void FillInfoArrays();
	void UpdateModelMatrice(Transform transform, mat4x4& translate, mat4x4& rotate, mat4x4& scale);
	void HandleVao();


	int iD;
	Transform transform;

	std::list<triangle> triangles;
	std::vector<Mesh> subMeshes;

	std::vector<float> positions;
	std::vector<float> uvs;
	std::vector<float> normals;


	GLuint VAO;

	Texture texture;
};