#pragma once

#include "Vulkan.hpp"
#include "KeyboardInput.hpp"
#include "MouseInput.hpp"

namespace vox {

class InputHandler
{
	public:

	InputHandler();
	~InputHandler() = default;
	InputHandler(const InputHandler& other) = delete;
	InputHandler& operator=(const InputHandler& other) = delete;

	

	private:

	KeyboardInput	keyboard;
	MouseInput		mouse;
};

} // namespace vox
