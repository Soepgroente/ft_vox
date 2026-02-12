#pragma once

#include "VulkanObject.hpp"
#include "VulkanModel.hpp"

#include <GLFW/glfw3.h>

namespace ve {

class KeyboardInput
{
	public:

	KeyboardInput() = delete;
	~KeyboardInput() = default;
	KeyboardInput(GLFWwindow* window);
	KeyboardInput(const KeyboardInput&) = delete;
	KeyboardInput& operator=(const KeyboardInput&) = delete;

	struct KeyMappings
	{
		int moveLeft;
		int moveRight;
		int moveForward;
		int moveBackward;
		int moveUp;
		int moveDown;
		int lookLeft;
		int lookRight;
		int lookUp;
		int lookDown;
		int rotate;
		int	toggleTexture;
		std::vector<size_t>	lastPressedFrames;
	};

	bool	shouldRotate(size_t frameCount, bool isRotating);
	bool	shouldShowTextures(size_t frameCount, bool isShowing);

	void	move(VulkanObject& object, float deltaTime);

	KeyMappings	keys;

	float	movementSpeed{10.0f};
	float	lookSpeed{1.0f};

	private:

	GLFWwindow*	window;
};

} // namespace ve