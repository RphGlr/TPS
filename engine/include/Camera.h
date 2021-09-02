#pragma once
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "Types.h"
#include "Component.h"

class Camera : public Component
{
protected:
	struct CameraInputs
	{
		bool mouseCaptured = false;
		double mouseX = 0.0;
		double mouseY = 0.0;
		float mouseDeltaX = 0.0;
		float mouseDeltaY = 0.0;

		float deltaX = 0.0;
		float deltaY = 0.0;
		bool rotateRight = false;
		bool rotateLeft = false;
	};

public:
	Camera(GameObject& gameObject);
	Camera(GameObject& gameObject, int width, int height, float3 position = { 0.f,0.f,0.f });
	Camera(GameObject& gameObject, std::shared_ptr<Camera> ptr);

	void Init();
	void HandleCameraInput(float deltaTime);
	void Update(float deltaTime);

	void UpdatePosition();

	mat4x4 getViewMatrix();
	mat4x4 getProjection();

	void mouseMovements(bool value);
	float3 GetForwardVector(bool xZPlane = false);

	CameraInputs cInputs = CameraInputs();

	Transform transform;
	float3 position = { 0.f, 0.f, 0.f };

	double mouseX = 0.0;
	double mouseY = 0.0;
	float mouseDeltaX = 0.0;
	float mouseDeltaY = 0.0;

	float pitch = 0.f;
	float yaw = 0.f;

	float shakeAmount;

	float speed = 5.f;

	//Camera Offsets

	float pitchOffset = 0.0;
	float yawOffset = 0.0;

private:
	
	float aspect = 1.f;
	float fovY = Maths::toRadians(60.f);
	float near = 0.01f;
	float far = 10000.0f;
};

