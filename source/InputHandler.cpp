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
			case GLFW_PRESS:
				handler->keyboard.keysPressed[key] = true; break;
			case GLFW_RELEASE:
				handler->keyboard.keysReleased[key] = true;
				handler->keyboard.keysPressed[key] = false;
				break;
			case GLFW_REPEAT: handler->keyboard.keysRepeated[key] = true; break;
			default: break;
		}

		if (handler->isKeyPressed(GLFW_KEY_T) == true)
			handler->toggleCursorFocus(window);
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
	glfwSetCursorPosCallback(window, [](GLFWwindow* window, double posX, double posY) {
		if (glfwGetWindowAttrib(window, GLFW_FOCUSED) == false)
			return;
		InputHandler* handler = static_cast<InputHandler*>(glfwGetWindowUserPointer(window));
		handler->setCursorPos(static_cast<float>(posX), static_cast<float>(posY));
	});
}

void InputHandler::reset()
{
	keyboard.reset();
	mouse.reset();
}

void InputHandler::setCursorPos( float posX, float posY ) {
	this->mouse.setCursorPos(posX, posY);
}

bool InputHandler::cursorPositionHasChanged( float& deltaX, float& deltaY ) noexcept {
	// if mouse is not focused do not update the change in the cursor pos
	if (this->cursorFocus == false)
		return false;
	return this->mouse.cursorPositionHasChanged(deltaX, deltaY);
}

void InputHandler::toggleCursorFocus( GLFWwindow* window) noexcept {
	this->cursorFocus = !this->cursorFocus; 
	if (this->cursorFocus)
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	else
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}


}	// namespace vox
