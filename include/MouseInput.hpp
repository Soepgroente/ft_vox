#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <array>


namespace vox {

class MouseInput
{
	public:

	MouseInput() noexcept : posX(0.0f), posY(0.0f) { this->reset(); };
	~MouseInput() noexcept = default;
	MouseInput(const MouseInput& other) = delete;
	MouseInput(MouseInput&& other) = delete;
	MouseInput& operator=(const MouseInput& other) = delete;
	MouseInput& operator=(MouseInput&& other) = delete;

	void	reset() noexcept;
	void	setCursorPos( float, float ) noexcept;
	void	getCursorPos( float&, float& ) const noexcept;

	static constexpr int maxButtons = GLFW_MOUSE_BUTTON_LAST + 1;

	std::array<bool, maxButtons>	mouseStates;
	std::array<bool, maxButtons>	buttonsPressed;
	std::array<bool, maxButtons>	buttonsReleased;

	float	posX;
	float	posY;
};

}	// namespace vox
