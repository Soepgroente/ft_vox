#pragma once

#include "KeyboardInput.hpp"
#include "MouseInput.hpp"

#include <functional>


namespace vox {

class InputHandler
{
	public:

	InputHandler(std::function<void(float, float)> mouseCb, std::function<void(int32_t, int32_t)> resizeCb) noexcept :
		fpsMode(false),
		mouseCallback(mouseCb),
		resizeCallback(resizeCb) {};
	~InputHandler() noexcept = default;
	InputHandler(const InputHandler&) = delete;
	InputHandler& operator=(const InputHandler&) = delete;

	void	setCallbacks(GLFWwindow* window);
	void	reset() noexcept;

	bool	isKeyPressed(int key) const { return keyboard.keysPressed[key]; }
	bool	isKeyReleased(int key) const { return keyboard.keysReleased[key]; }
	bool	isKeyRepeated(int key) const { return keyboard.keysRepeated[key]; }
	bool	isMouseButtonPressed(int button) const { return mouse.buttonsPressed[button]; }
	bool	isMouseButtonReleased(int button) const { return mouse.buttonsReleased[button]; }
	void	setCursorPos( float, float ) noexcept;
	void	getCursorPos( float&, float& ) const noexcept;
	void	toggleFpsMode( GLFWwindow* ) noexcept;
	void	closeWindow( GLFWwindow* ) const noexcept;

	private:

	KeyboardInput	keyboard;
	MouseInput		mouse;
	bool			fpsMode;

	std::function<void(float, float)>		mouseCallback;
	std::function<void(int32_t, int32_t)>	resizeCallback;
};

} // namespace vox
