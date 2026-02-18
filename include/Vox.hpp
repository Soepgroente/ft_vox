#pragma once

#include "Vectors.hpp"
#include "Vulkan.hpp"
#include "Config.hpp"
#include "InputHandler.hpp"

#include <array>
#include <memory>
#include <vector>

namespace vox {

class Vox
{
	public:

	bool initialize( void );
	void run( void );
	void shutdown( void );

	private:

	void	loadObjects();

	ve::VulkanWindow	vulkanWindow{Config::defaultWindowHeight, Config::defaultWindowWidth, "Vox"};
	ve::VulkanDevice	vulkanDevice{vulkanWindow};
	ve::VulkanRenderer	vulkanRenderer{vulkanWindow, vulkanDevice};
	std::unique_ptr<ve::VulkanDescriptorPool>	globalDescriptorPool{};

	std::string					objModelPath;
	ve::VulkanObject::Map			objects;
	std::vector<ve::VulkanTexture>	textures;
	InputHandler	inputHandler;
};

}	// namespace vox
