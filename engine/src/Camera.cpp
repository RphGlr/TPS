//#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui.h>

#include "Input.h"
#include "Camera.h"

using namespace Maths;

Camera::Camera(GameObject& gameObject) : Camera(gameObject, std::shared_ptr<Camera>(this))
{
}

Camera::Camera(GameObject& gameObject, int width, int height, float3 position) : Camera(gameObject, std::shared_ptr<Camera>(this))
{
	aspect = (float)width / (float)height;
	transform.position = position;
}

Camera::Camera(GameObject& gameObject, std::shared_ptr<Camera> ptr) : Component(gameObject,  ptr)
{
}

using namespace Core;

void Camera::mouseMovements(bool value)
{
	GLFWwindow* window = Input::instance()->window;

	cInputs.mouseCaptured = value;
	glfwSetInputMode(window, GLFW_CURSOR, value == true ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);

	if (cInputs.mouseCaptured)
	{
		double newMouseX, newMouseY;
		glfwGetCursorPos(window, &newMouseX, &newMouseY);
		cInputs.mouseX = newMouseX;
		cInputs.mouseY = newMouseY;
	}
}

float3 Camera::GetForwardVector(bool xZPlane)
{
	float3 res = Vector3RotateByQuaternion({ 0,0,-1 }, QuaternionFromEuler(-pitch, -yaw, 0));

	res.y = xZPlane ? 0 : res.y;

	return res;
}

void Camera::Init()
{
	Input::instance()->mousePressed.find(GLFW_MOUSE_BUTTON_RIGHT)->second = [&] { Camera::mouseMovements(true); };
	Input::instance()->mouseReleased.find(GLFW_MOUSE_BUTTON_RIGHT)->second = [&] { Camera::mouseMovements(false); };
}

void Camera::HandleCameraInput(float deltaTime)
{
	GLFWwindow* window = Input::instance()->window;

	const float MOUSE_SENSITIVITY = 0.001f;

	cInputs.deltaX = 0;
	cInputs.deltaY = 0;

	if (cInputs.mouseCaptured)
	{
		double newMouseX, newMouseY;

		float mouseDeltaX = 0.f;
		float mouseDeltaY = 0.f;

		glfwGetCursorPos(window, &newMouseX, &newMouseY);
		mouseDeltaX = (float)(newMouseX - cInputs.mouseX);
		mouseDeltaY = (float)(newMouseY - cInputs.mouseY);

		cInputs.mouseX = newMouseX;
		cInputs.mouseY = newMouseY;

		cInputs.deltaX = mouseDeltaX;
		cInputs.deltaY = mouseDeltaY;
	}

	yaw += cInputs.deltaX * MOUSE_SENSITIVITY;
	pitch += cInputs.deltaY * MOUSE_SENSITIVITY;

	pitch = pitch < 0 ? 0 : pitch;
}

void Camera::Update(float deltaTime)
{
	GLFWwindow* window = Input::instance()->window;

	int width, height;
	glfwGetWindowSize(window, &width, &height);
	aspect = (float)width / (float)height;
	
	HandleCameraInput(deltaTime);

	UpdatePosition();

}

void Camera::UpdatePosition()
{
	float3 forwardVect = GetForwardVector();

	transform.position = cgameObject.transform.position - forwardVect * 25;


	float3 shakeOffset = float3{ (float)(rand()%100)/100.f,(float)(rand() % 100) / 100.f, (float)(rand() % 100) / 100.f };
	shakeOffset = shakeOffset * shakeAmount;

	transform.position = transform.position + shakeOffset;

}

mat4x4 Camera::getViewMatrix()
{
	return rotateX(pitch) * rotateY(yaw) * Translate( - transform.position);
}
mat4x4 Camera::getProjection()
{
	return perspective(fovY, aspect, near, far);
}
