#pragma once

#include "Vectors.hpp"
#include "VulkanWindow.hpp"
#include "VulkanObject.hpp"

namespace ve {

struct MousePos
{
	double	x;
	double	y;
};

class	MouseInput
{
	public:

	MouseInput() = delete;
	~MouseInput() = default;
	MouseInput(GLFWwindow* window);
	MouseInput(const MouseInput&) = delete;
	MouseInput& operator=(const MouseInput&) = delete;

	void	move(VulkanObject& object, float deltaTime);

	private:

	GLFWwindow*	window;
	float		sensitivity{0.02f};
	MousePos	lastMousePos{0.0, 0.0};
	bool		firstMouseFrame{true};
	int			inputMode{GLFW_CURSOR_NORMAL};
};

} // namespace ve