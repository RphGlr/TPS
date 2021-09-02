#pragma once

#include "singleton.h"
#include <string>
#include <functional>
#include <unordered_map>
#include <iostream>

struct GLFWwindow;

namespace Core
{
	using namespace std;

	class Input : public Singleton<Input>
	{
	private:

		static void onButtonPressStatic(GLFWwindow* window, int key, int scancode, int action, int mods);
		void onButtonPress(GLFWwindow* window, int key, int scancode, int action, int mods);

		static void onMousePressStatic(GLFWwindow* window, int button, int action, int mods);
		void onMousePress(GLFWwindow* window, int button, int action, int mods);

	public:
		GLFWwindow* window = nullptr;

		std::unordered_map<int, std::function<void(void)>> mousePressed;
		std::unordered_map<int, std::function<void(void)>> mouseReleased;
		std::unordered_map<int, std::function<void(void)>> actionsPressed;
		std::unordered_map<int, std::function<void(void)>> actionsReleased;
		Input();

		void initialize(GLFWwindow* window);

		void update();
	};
}