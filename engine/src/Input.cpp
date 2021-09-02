#include "Input.h"

#include <GLFW/glfw3.h>

// Static callback function to make it working with glfwSetKeyCallback
void Core::Input::onButtonPressStatic(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	Input* input = instance();
	input->onButtonPress(window, key, scancode, action, mods);
}
// Callback function
void Core::Input::onButtonPress(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS)
	{
		if (actionsPressed.contains(key))
			actionsPressed[key]();
	}
	else if (action == GLFW_RELEASE)
	{
		if (actionsReleased.contains(key))
			actionsReleased[key]();
	}
}

void Core::Input::onMousePressStatic(GLFWwindow* window, int button, int action, int mods)
{
	Input* input = instance();
	input->onMousePress(window, button, action, mods);
}


void Core::Input::onMousePress(GLFWwindow* window, int button, int action, int mods)
{
	if (action == GLFW_PRESS)
	{
		if (mousePressed.contains(button))
			mousePressed[button]();
	}
	else if (action == GLFW_RELEASE)
	{
		if (mouseReleased.contains(button))
			mouseReleased[button]();
	}
}

Core::Input::Input()
{
}

void nothing() {}

void Core::Input::initialize(GLFWwindow* window)
{
	this->window = window;
	// Setting up actions for different keys
	actionsPressed =
	{
		{GLFW_KEY_ESCAPE, nothing},
		{GLFW_KEY_W, nothing},
		{GLFW_KEY_S, nothing},
		{GLFW_KEY_A, nothing},
		{GLFW_KEY_D, nothing},
		{GLFW_KEY_Z, nothing},
		{GLFW_KEY_Q, nothing},
		{GLFW_KEY_F5, nothing},
		{GLFW_KEY_UP, nothing},
		{GLFW_KEY_DOWN, nothing},
		{GLFW_KEY_LEFT, nothing},
		{GLFW_KEY_RIGHT, nothing},
		{GLFW_KEY_LEFT_SHIFT, nothing},
		{GLFW_KEY_LEFT_CONTROL, nothing},
		{GLFW_KEY_SPACE, nothing}
	};

	actionsReleased =
	{
		{GLFW_KEY_ESCAPE, nothing},
		{GLFW_KEY_W, nothing},
		{GLFW_KEY_S, nothing},
		{GLFW_KEY_A, nothing},
		{GLFW_KEY_D, nothing},
		{GLFW_KEY_Q, nothing},
		{GLFW_KEY_Z, nothing},
		{GLFW_KEY_F5, nothing},
		{GLFW_KEY_UP, nothing },
		{GLFW_KEY_DOWN, nothing},
		{GLFW_KEY_LEFT, nothing},
		{GLFW_KEY_RIGHT, nothing},
		{GLFW_KEY_LEFT_SHIFT, nothing},
		{GLFW_KEY_LEFT_CONTROL, nothing},
		{GLFW_KEY_SPACE, nothing}
	};

	mousePressed =
	{
		{GLFW_MOUSE_BUTTON_LEFT, nothing},
		{GLFW_MOUSE_BUTTON_RIGHT, nothing}
	};

	mouseReleased =
	{
		{GLFW_MOUSE_BUTTON_LEFT, nothing},
		{GLFW_MOUSE_BUTTON_RIGHT, nothing}
	};

	// Setting up the callback for GLFW
	glfwSetKeyCallback(window, onButtonPressStatic);

	glfwSetMouseButtonCallback(window, onMousePressStatic);
}

void Core::Input::update()
{

}
