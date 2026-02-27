#pragma once

#include "Vectors.hpp"
#include "Vulkan.hpp"
#include "InputHandler.hpp"
#include "VoxelWorld.hpp"
#include "Config.hpp"

#include <array>
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

		InputHandler const& getHandler( void ) const noexcept;
		InputHandler&		getHandler( void ) noexcept;

		void moveCamera( float );
		void rotateCamera( void );

	private:
		void	createObjects( void );

		ve::VulkanWindow	vulkanWindow{Config::defaultWindowHeight, Config::defaultWindowWidth, "Vox"};
		ve::VulkanDevice	vulkanDevice{vulkanWindow};
		ve::VulkanRenderer	vulkanRenderer{vulkanWindow, vulkanDevice};
		std::unique_ptr<ve::VulkanDescriptorPool>	globalDescriptorPool{};

		std::string						objModelPath;
		ve::Camera						camera;
		ve::VulkanObject::Map			objects;
		std::vector<ve::VulkanTexture>	textures;
		VoxelWorld						world;
		InputHandler					inputHandler;
};

}
