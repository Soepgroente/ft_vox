#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vector>
#include <functional>
#include <array>

namespace vox {

class MouseInput
{
	public:

	MouseInput();
	~MouseInput() = default;
	MouseInput(const MouseInput& other) = delete;
	MouseInput& operator=(const MouseInput& other) = delete;

	void	reset();
	void	setCursorPos( float, float );
	void	getCursorPos( float&, float& );

	static constexpr int maxButtons = GLFW_MOUSE_BUTTON_LAST + 1;

	std::array<bool, maxButtons>	mouseStates;
	std::array<bool, maxButtons>	buttonsPressed;
	std::array<bool, maxButtons>	buttonsReleased;
	float	posX;
	float	posY;
};

}	// namespace vox
