#pragma once

#include "Vulkan.hpp"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vector>
#include <functional>
#include <array>
#include <cmath>


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
	bool	cursorPositionHasChanged( float&, float& ) noexcept;

	static constexpr int maxButtons = GLFW_MOUSE_BUTTON_LAST + 1;

	std::array<bool, maxButtons>	mouseStates;
	std::array<bool, maxButtons>	buttonsPressed;
	std::array<bool, maxButtons>	buttonsReleased;
	float	posX;
	float	posY;
	float	lastPosX;
	float	lastPosY;
};

}	// namespace vox
