#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <stdexcept>
#include <string>

namespace ve {

class VulkanWindow
{
	public:

	VulkanWindow() = delete;
	VulkanWindow(int width, int height, const char* title);
	VulkanWindow(const VulkanWindow&) = delete;
	VulkanWindow& operator=(const VulkanWindow&) = delete;
	~VulkanWindow();

	bool	shouldClose() const noexcept { return glfwWindowShouldClose(window) || glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS; }
	bool	wasWindowResized() const noexcept { return resized; }
	void	resetWindowResizedFlag() noexcept { resized = false; }
	float	getAspectRatio() const noexcept { return static_cast<float>(width) / static_cast<float>(height); }
	VkExtent2D	getFramebufferExtent() const noexcept { return { static_cast<uint32_t>(width), static_cast<uint32_t>(height) }; }
	
	GLFWwindow*	getGLFWwindow() const noexcept { return window; }
	void	createWindowSurface(VkInstance instance, VkSurfaceKHR* surface);

	private:

	static void	framebufferResizeCallback(GLFWwindow* window, int width, int height);
	void	initWindow();

	int		width;
	int		height;
	bool	resized = false;

	std::string	title;
	GLFWwindow*	window;
};

}