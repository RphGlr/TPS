#pragma once
#include "Component.h"
#include "Material.h"
#include "Model.h"

#include <iostream>
#include <ctime>
#include <ratio>
#include <chrono>
using namespace std::chrono;

class RenderingResources : public Component
{
public:

	RenderingResources(GameObject& gameObject, Model model, Material material, Transform transform = Transform());
	void Draw(mat4x4& translate, mat4x4& rotate, mat4x4& scale, mat4x4& model);

	Model model;
	Material material;
	Transform transform;

	high_resolution_clock::time_point t1;

};