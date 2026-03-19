#include "InputHandler.hpp"
#include <cstring>

namespace vox {

void	InputHandler::setCallbacks(GLFWwindow* window)
{
	glfwSetWindowUserPointer(window, this);
	// keyboard key pression callback
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
		else if (handler->isKeyPressed(GLFW_KEY_ESCAPE) == true)
			handler->closeWindow(window);
	});
	// mouse key pression callback
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
	// mouse scroll callback
	glfwSetScrollCallback(window, [](GLFWwindow* window, double xoffset, double yoffset)
	{
		(void)xoffset;
		(void)yoffset;
		InputHandler* handler = static_cast<InputHandler*>(glfwGetWindowUserPointer(window));
		(void)handler;
		(void)window;
	});
	// changed cursor position callback
	glfwSetCursorPosCallback(window, [](GLFWwindow* window, double posX, double posY) {
		if (glfwGetWindowAttrib(window, GLFW_FOCUSED) == false)
			return;
		InputHandler* handler = static_cast<InputHandler*>(glfwGetWindowUserPointer(window));
		float posXf = static_cast<float>(posX);
		float posYf = static_cast<float>(posY);
		if (handler->cursorFocus == true)
			handler->mouseCallback(posXf, posYf);
		handler->setCursorPos(posXf, posYf);
	});
	// window resize callback
	glfwSetFramebufferSizeCallback(window, [](GLFWwindow* window, int32_t w, int32_t h) {
		InputHandler* handler = static_cast<InputHandler*>(glfwGetWindowUserPointer(window));
		handler->resizeCallback(w, h);
	});
}

void InputHandler::reset() noexcept
{
	keyboard.reset();
	mouse.reset();
}

void InputHandler::setCursorPos( float posX, float posY ) noexcept {
	this->mouse.setCursorPos(posX, posY);
}

void InputHandler::getCursorPos( float &posX, float &posY ) noexcept {
	this->mouse.getCursorPos(posX, posY);
}

void InputHandler::toggleCursorFocus( GLFWwindow* window ) noexcept {
	this->cursorFocus = !this->cursorFocus; 
	glfwSetInputMode(window, GLFW_CURSOR, this->cursorFocus ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
}

void InputHandler::closeWindow( GLFWwindow* window ) noexcept {
	glfwSetWindowShouldClose(window, GLFW_TRUE);
}

}	// namespace vox
