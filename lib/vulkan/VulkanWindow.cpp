#include "VulkanWindow.hpp"
#include <iostream>

namespace ve {

VulkanWindow::VulkanWindow(int width, int height, const char* title) : width(width), height(height), title(title)
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
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

	window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
	if (window == nullptr)
	{
		throw std::runtime_error("failed to create GLFW window");
	}
	glfwSetWindowUserPointer(window, this);
	glfwSetFramebufferSizeCallback(window, &framebufferResizeCallback);
}

void	VulkanWindow::createWindowSurface(VkInstance instance, VkSurfaceKHR* surface)
{
	if (glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create window surface");
	}
}

void	VulkanWindow::framebufferResizeCallback(GLFWwindow* window, int width, int height)
{
	VulkanWindow*	vulkanWindow = reinterpret_cast<VulkanWindow*>(glfwGetWindowUserPointer(window));

	vulkanWindow->resized = true;
	vulkanWindow->width = width;
	vulkanWindow->height = height;
}

}	// namespace ve