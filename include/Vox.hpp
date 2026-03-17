#pragma once

#include "Vulkan.hpp"
#include "Vectors.hpp"
#include "Config.hpp"
#include "World.hpp"
#include "InputHandler.hpp"

#include <cstdint>


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

		void moveCamera( float );
		void rotateCameraFromCursorPos( float, float );
		void resizeWindow( uint32_t, uint32_t );

	private:
		ve::VulkanWindow	vulkanWindow{Config::defaultWindowHeight, Config::defaultWindowWidth, "Vox"};
		ve::VulkanDevice	vulkanDevice{vulkanWindow};
		ve::VulkanRenderer	vulkanRenderer{vulkanWindow, vulkanDevice};
		std::unique_ptr<ve::VulkanDescriptorPool>	globalDescriptorPool{};

		ve::Camera		camera{Config::startingPos, ve::CameraSettings::cameraForward, Config::cameraLimitsMov};
		WorldNavigator	navigator{Config::worldSize};
		InputHandler	inputHandler;
};

}	// namespace vox
