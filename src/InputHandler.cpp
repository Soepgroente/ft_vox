#include "InputHandler.hpp"
#include <cstring>

namespace vox {

void	InputHandler::setCallbacks(GLFWwindow* window)
{
	glfwSetWindowUserPointer(window, this);
	glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		(void)scancode;
		(void)mods;
		InputHandler* handler = static_cast<InputHandler*>(glfwGetWindowUserPointer(window));

		switch(action)
		{
			case GLFW_PRESS: handler->keyboard.keysPressed[key] = true; break;
			case GLFW_RELEASE: handler->keyboard.keysReleased[key] = true; break;
			case GLFW_REPEAT: handler->keyboard.keysRepeated[key] = true; break;
			default: break;
		}
	});
	glfwSetMouseButtonCallback(window, [](GLFWwindow* window, int button, int action, int mods)
	{
		(void)mods;
		InputHandler* handler = static_cast<InputHandler*>(glfwGetWindowUserPointer(window));

		switch(action)
		{
			case GLFW_PRESS: handler->mouse.buttonsPressed[button] = true; break;
			case GLFW_RELEASE: handler->mouse.buttonsReleased[button] = true; break;
			default: break;
		}
	});
	glfwSetScrollCallback(window, [](GLFWwindow* window, double xoffset, double yoffset)
	{
		(void)xoffset;
		(void)yoffset;
		InputHandler* handler = static_cast<InputHandler*>(glfwGetWindowUserPointer(window));
		(void)handler;
		(void)window;
		// Handle scroll input here if needed
	});
}

void	InputHandler::reset()
{
	keyboard.reset();
	mouse.reset();
}

}	// namespace vox
