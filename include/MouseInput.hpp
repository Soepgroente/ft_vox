#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <array>


namespace vox {

class MouseInput
{
	public:

	MouseInput() noexcept { this->reset(); };
	~MouseInput() noexcept = default;
	MouseInput(const MouseInput& other) = delete;
	MouseInput& operator=(const MouseInput& other) = delete;

	void	reset() noexcept;
	void	setCursorPos( float, float ) noexcept;
	void	getCursorPos( float&, float& ) noexcept;

	static constexpr int maxButtons = GLFW_MOUSE_BUTTON_LAST + 1;

	std::array<bool, maxButtons>	mouseStates;
	std::array<bool, maxButtons>	buttonsPressed;
	std::array<bool, maxButtons>	buttonsReleased;

	float	posX = 0.0f;;
	float	posY = 0.0f;;
};

}	// namespace vox
