#pragma once

#include <vector>
#include <functional>
#include <array>
#include <GLFW/glfw3.h>

#include "Vulkan.hpp"

namespace vox {

class MouseInput
{
	public:

	MouseInput();
	~MouseInput() = default;
	MouseInput(const MouseInput& other) = delete;
	MouseInput& operator=(const MouseInput& other) = delete;

	void	reset();

	static constexpr int maxButtons = GLFW_MOUSE_BUTTON_LAST + 1;

	std::array<bool, maxButtons>	mouseStates;
	std::array<bool, maxButtons>	buttonsPressed;
	std::array<bool, maxButtons>	buttonsReleased;
};

}	// namespace vox
