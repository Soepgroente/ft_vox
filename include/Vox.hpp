#pragma once

#include "Vectors.hpp"
#include "Vulkan.hpp"
#include "InputHandler.hpp"
#include "VoxelWorld.hpp"
#include "Config.hpp"

#include <array>
#include <cmath>
#include <memory>
#include <vector>

namespace vox {

class Vox
{
	public:
		Vox( void );
		~Vox( void );
		Vox( Vox const& ) = delete;
		Vox( Vox&& ) = delete;
		Vox& operator=( Vox const& ) = delete;
		Vox& operator=( Vox&& ) = delete;

		void run( void );
		void shutdown( void );

		void moveCamera( float );
		void rotateCameraFromCursorPos( float, float );
		void resizeWindow( uint32_t, uint32_t );

	private:
		ve::VulkanWindow	vulkanWindow{Config::defaultWindowHeight, Config::defaultWindowWidth, "Vox"};
		ve::VulkanDevice	vulkanDevice{vulkanWindow};
		ve::VulkanRenderer	vulkanRenderer{vulkanWindow, vulkanDevice};
		std::unique_ptr<ve::VulkanDescriptorPool>	globalDescriptorPool{};

		std::string						objModelPath;
		std::vector<ve::VulkanTexture>	textures;
		ve::Camera						camera;
		WorldGenerator					world;
		InputHandler					inputHandler;
};


}
