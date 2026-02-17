#pragma once

#include <vector>
#include <functional>
#include <array>
#include <GLFW/glfw3.h>

#include "Vulkan.hpp"

namespace vox {

class KeyboardInput
{
	public:

	KeyboardInput();
	~KeyboardInput() = default;
	KeyboardInput(const KeyboardInput& other) = delete;
	KeyboardInput& operator=(const KeyboardInput& other) = delete;

	void	reset();

	static constexpr int maxKeys = GLFW_KEY_LAST + 1;

	std::array<bool, maxKeys>	keyStates;
	std::array<bool, maxKeys>	keysPressed;
	std::array<bool, maxKeys>	keysReleased;
	std::array<bool, maxKeys>	keysRepeated;
};

}	// namespace vox
