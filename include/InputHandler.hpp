#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "KeyboardInput.hpp"
#include "MouseInput.hpp"

namespace vox {

class InputHandler
{
	public:

	InputHandler() = default;
	~InputHandler() = default;
	InputHandler(const InputHandler& other) = delete;
	InputHandler& operator=(const InputHandler& other) = delete;

	void	setCallbacks(GLFWwindow* window);
	void	reset();

	bool	isKeyPressed(int key) const { return keyboard.keysPressed[key]; }
	bool	isKeyReleased(int key) const { return keyboard.keysReleased[key]; }
	bool	isKeyRepeated(int key) const { return keyboard.keysRepeated[key]; }
	bool	isMouseButtonPressed(int button) const { return mouse.buttonsPressed[button]; }
	bool	isMouseButtonReleased(int button) const { return mouse.buttonsReleased[button]; }
	void	setCursorPos( float, float );
	bool	cursorPositionHasChanged( float&, float& ) noexcept;
	void	toggleCursorFocus( GLFWwindow* ) noexcept;

	private:

	KeyboardInput	keyboard;
	MouseInput		mouse;
	bool			cursorFocus = false;
};

} // namespace vox
