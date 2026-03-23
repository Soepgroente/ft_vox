#pragma once

#include "Vulkan.hpp"
#include "Vectors.hpp"
#include "Config.hpp"
#include "World.hpp"
#include "InputHandler.hpp"
#include "ThreadManager.hpp"

#include <cstdint>
#include <thread>


namespace vox {

using ui32 = uint32_t;

enum TextureTypes {
	TEXT_DIRT_1,
	TEXT_DIRT_2,
	TEXT_STONE_1,
	TEXT_STONE_2
};

class Vox
{
	public:
		Vox( void );
		~Vox( void ) noexcept;
		Vox( Vox const& ) = delete;
		Vox( Vox&& ) = delete;
		Vox& operator=( Vox const& ) = delete;
		Vox& operator=( Vox&& ) = delete;

		void run( void );

		void moveCamera( float );
		void rotateCameraFromCursorPos( vec2 const& );
		void resizeWindow( ui32, ui32 );

		static std::vector<std::thread>	workerThreads;

	private:
		ve::VulkanWindow							vulkanWindow;
		ve::VulkanDevice							vulkanDevice;
		ve::VulkanRenderer							vulkanRenderer;
		std::unique_ptr<ve::VulkanDescriptorPool>	globalDescriptorPool;

		ve::Camera		camera;
		WorldNavigator	navigator;
		InputHandler	inputHandler;
		ThreadManager	threadManager;

		std::map<TextureTypes,ve::VulkanTexture> textures;
};

}	// namespace vox
