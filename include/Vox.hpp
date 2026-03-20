#pragma once

#include "Vulkan.hpp"
#include "Vectors.hpp"
#include "Config.hpp"
#include "World.hpp"
#include "InputHandler.hpp"

#include <cstdint>
#include <thread>


namespace vox {

using ui32 = uint32_t;

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
		void resizeWindow( uint32_t, uint32_t );

		static std::vector<std::thread>	workerThreads;

	private:
		ve::VulkanWindow							vulkanWindow;
		ve::VulkanDevice							vulkanDevice;
		ve::VulkanRenderer							vulkanRenderer;
		std::unique_ptr<ve::VulkanDescriptorPool>	globalDescriptorPool;

		ve::Camera		camera;
		WorldNavigator	navigator;
		InputHandler	inputHandler;
};

}	// namespace vox
