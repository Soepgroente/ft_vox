#include "KeyboardInput.hpp"
#include "Vectors.hpp"

namespace ve {

KeyboardInput::KeyboardInput(GLFWwindow* window) : window{window}
{
	keys.moveLeft = GLFW_KEY_A;
	keys.moveRight = GLFW_KEY_D;
	keys.moveForward = GLFW_KEY_W;
	keys.moveBackward = GLFW_KEY_S;
	keys.moveUp = GLFW_KEY_E;
	keys.moveDown = GLFW_KEY_Q;
	keys.lookLeft = GLFW_KEY_LEFT;
	keys.lookRight = GLFW_KEY_RIGHT;
	keys.lookUp = GLFW_KEY_UP;
	keys.lookDown = GLFW_KEY_DOWN;
	keys.rotate = GLFW_KEY_KP_ADD;
	keys.toggleTexture = GLFW_KEY_T;
	keys.lastPressedFrames.resize(12, 0);
}

bool	KeyboardInput::shouldRotate(size_t frameCount, bool isRotating)
{
	if (keys.lastPressedFrames[10] + 10 < frameCount && glfwGetKey(window, keys.rotate) == GLFW_PRESS)
	{
		keys.lastPressedFrames[10] = frameCount;
		return !isRotating;
	}
	return isRotating;
}

bool	KeyboardInput::shouldShowTextures(size_t frameCount, bool isShowing)
{
	if (keys.lastPressedFrames[11] + 10 < frameCount && glfwGetKey(window, keys.toggleTexture) == GLFW_PRESS)
	{
		keys.lastPressedFrames[11] = frameCount;
		return !isShowing;
	}
	return isShowing;
}

void	KeyboardInput::move(VulkanObject& object, float deltaTime)
{
	vec3	rotation{0.0f};

	if (glfwGetKey(window, keys.lookRight) == GLFW_PRESS) rotation.y += 1.0f;
	if (glfwGetKey(window, keys.lookLeft) == GLFW_PRESS) rotation.y -= 1.0f;
	if (glfwGetKey(window, keys.lookUp) == GLFW_PRESS) rotation.x -= 1.0f;
	if (glfwGetKey(window, keys.lookDown) == GLFW_PRESS) rotation.x += 1.0f;

	if (vec3::dot(rotation, rotation) > std::numeric_limits<float>::epsilon())
	{
		object.transform.rotation += rotation.normalize() * lookSpeed * deltaTime;
	}

	object.transform.rotation.x = std::clamp(object.transform.rotation.x, -half_pi() + 0.01f, half_pi() - 0.01f);
	object.transform.rotation.y = std::fmod(object.transform.rotation.y, two_pi());

	float	rot = object.transform.rotation.y;

	const vec3	forwardDir{std::sin(rot), 0.0f, std::cos(rot)};
	const vec3	rightDir{forwardDir.z, 0.0f, -forwardDir.x};
	const vec3	upDir{0.0f, -1.0f, 0.0f};

	vec3	moveDir{0.0f};

	if (glfwGetKey(window, keys.moveForward) == GLFW_PRESS) moveDir += forwardDir;
	if (glfwGetKey(window, keys.moveBackward) == GLFW_PRESS) moveDir -= forwardDir;
	if (glfwGetKey(window, keys.moveRight) == GLFW_PRESS) moveDir += rightDir;
	if (glfwGetKey(window, keys.moveLeft) == GLFW_PRESS) moveDir -= rightDir;
	if (glfwGetKey(window, keys.moveUp) == GLFW_PRESS) moveDir -= upDir;
	if (glfwGetKey(window, keys.moveDown) == GLFW_PRESS) moveDir += upDir;

	if (vec3::dot(moveDir, moveDir) > std::numeric_limits<float>::epsilon())
	{
		object.transform.translation += moveDir.normalize() * movementSpeed * deltaTime;
	}
}

} // namespace ve