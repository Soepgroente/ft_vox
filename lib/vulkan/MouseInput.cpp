#include "MouseInput.hpp"

namespace ve {

MouseInput::MouseInput(GLFWwindow* window) : window{window}
{
}

void	MouseInput::move(VulkanObject& object, float deltaTime)
{
	MousePos	currentMousePos;
	MousePos	mouseDelta;

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
	{
		inputMode = GLFW_CURSOR_NORMAL;
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}
	else if (inputMode != GLFW_CURSOR_DISABLED)
	{
		inputMode = GLFW_CURSOR_DISABLED;
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
	}
	if (inputMode == GLFW_CURSOR_NORMAL ||
		glfwGetWindowAttrib(window, GLFW_FOCUSED) == GLFW_FALSE ||
		glfwGetWindowAttrib(window, GLFW_HOVERED) == GLFW_FALSE)
	{
		firstMouseFrame = true;
		return;
	}
	glfwGetCursorPos(window, &currentMousePos.x, &currentMousePos.y);

	if (firstMouseFrame == true)
	{
		lastMousePos = currentMousePos;
		firstMouseFrame = false;
	}

	mouseDelta = {currentMousePos.x - lastMousePos.x, currentMousePos.y - lastMousePos.y};
	lastMousePos = currentMousePos;

	object.transform.rotation.y += static_cast<float>(mouseDelta.x) * sensitivity * deltaTime;
	object.transform.rotation.x += static_cast<float>(-mouseDelta.y) * sensitivity * deltaTime;

	object.transform.rotation.x = std::clamp(object.transform.rotation.x, -half_pi() + 0.01f, half_pi() - 0.01f);
	object.transform.rotation.y = std::fmod(object.transform.rotation.y, two_pi());
}

} // namespace ve