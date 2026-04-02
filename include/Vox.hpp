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
using i32 = int32_t;

enum TextureType {
	TEXT_DIRT_1,
	TEXT_DIRT_2,
	TEXT_STONE_1,
	TEXT_STONE_2,
	TEXT_SKYBOX
};


struct TerrainUBO
{
	mat4				model{1.0f};
	mat4				view{1.0f};
	mat4				projection{1.0f};
};

struct SkyboxUBO
{
	mat4				view{1.0f};
	mat4				projection{1.0f};
};


class Vox
{
	public:
		Vox( void );
		~Vox( void ) noexcept {};
		Vox( Vox const& ) = delete;
		Vox( Vox&& ) = delete;
		Vox& operator=( Vox const& ) = delete;
		Vox& operator=( Vox&& ) = delete;

		void setupVulkan( void );
		void run( void );

		void moveCamera( float );
		void rotateCameraFromCursorPos( vec2 const& );
		void resizeWindow( ui32, ui32 );
		std::unique_ptr<ve::VulkanModel> createSkyboxModel( void );

		static std::vector<std::thread>	workerThreads;

	private:
		ve::VulkanWindow	vulkanWindow;
		ve::VulkanDevice	vulkanDevice;
		ve::VulkanRenderer	vulkanRenderer;

		ve::Camera		camera;
		WorldNavigator	navigator;
		InputHandler	inputHandler;
		ThreadManager	threadManager;

		std::map<TextureType,ve::VulkanTexture>		textures;
		std::vector<std::unique_ptr<ve::VulkanBuffer>>	terrainUboBuffers;
		std::vector<std::unique_ptr<ve::VulkanBuffer>>	skyboxUboBuffers;

		std::unique_ptr<ve::VulkanDescriptorSet> terrainDescriptorSet;
		std::unique_ptr<ve::VulkanDescriptorSet> skyboxDescriptorSet;

		std::unique_ptr<ve::VulkanRenderSystem> terrainPipeline;
		std::unique_ptr<ve::VulkanRenderSystem> skyboxPipeline;
};

}	// namespace vox
