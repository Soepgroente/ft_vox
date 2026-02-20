#pragma once

#include "Vectors.hpp"
#include "Vulkan.hpp"
#include "VoxelWorld.hpp"

#include <array>
#include <memory>
#include <vector>

namespace vox {

class Vox
{
	public:
		Vox( void ) = delete;
		Vox( std::string const& );
		~Vox( void );

		Vox( Vox const& ) = delete;
		Vox& operator=( Vox const& ) = delete;

		bool initialize( void );
		void run( void );
		void shutdown( void );

		static constexpr int	DEFAULT_WIDTH = 1080;
		static constexpr int	DEFAULT_HEIGHT = 1080;

	private:
		void	loadObjects( void );

		ve::VulkanWindow	vulkanWindow{DEFAULT_HEIGHT, DEFAULT_WIDTH, "Vox"};
		ve::VulkanDevice	vulkanDevice{vulkanWindow};
		ve::VulkanRenderer	vulkanRenderer{vulkanWindow, vulkanDevice};
		std::unique_ptr<ve::VulkanDescriptorPool>	globalDescriptorPool{};

		std::string					objModelPath;
		ve::VulkanObject::Map			objects;
		std::vector<ve::VulkanTexture>	textures;
		VoxelWorld					world;
};

std::vector<ve::ObjInfo>	parseOBJFile( std::string const& objFilePath ); 
}
