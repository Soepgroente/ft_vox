#include "VulkanWindow.hpp"
#include <iostream>

namespace ve {

VulkanWindow::VulkanWindow(const char* title, bool fullScreen, int width, int height) :
	title(title),
	fullScreen(fullScreen),
	width(width),
	height(height)
{
	initWindow();
}

VulkanWindow::~VulkanWindow()
{
	glfwDestroyWindow(window);
	glfwTerminate();
}

void	VulkanWindow::initWindow()
{
	if (glfwInit() == GLFW_FALSE)
	{
		throw std::runtime_error("failed to start GLFW");
	}

	GLFWmonitor* monitor = nullptr;
	if (fullScreen == true)
	{
		monitor = glfwGetPrimaryMonitor();
		const GLFWvidmode* mode = glfwGetVideoMode(monitor);
		
		width = mode->width;
		height = mode->height;
		glfwWindowHint(GLFW_RED_BITS, mode->redBits);
		glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
		glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
		glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	}
	else
	{
		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
	}
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	window = glfwCreateWindow(width, height, title.c_str(), monitor, nullptr);
	if (window == nullptr)
	{
		throw std::runtime_error("failed to create GLFW window");
	}
}

void	VulkanWindow::createWindowSurface(VkInstance instance, VkSurfaceKHR* surface)
{
	if (glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create window surface");
	}
}

void	VulkanWindow::resetWindowSize(int width, int height)
{
	this->resized = true;
	this->width = width;
	this->height = height;
}

}	// namespace ve