#pragma once

#include "Vulkan.hpp"
#include "Camera.hpp"
#include "Config.hpp"
#include "InputHandler.hpp"
#include "ThreadManager.hpp"
#include "VoxelMap.hpp"
#include "TypeAliases.hpp"

#include <array>
#include <memory>
#include <vector>
#include <climits>


namespace vox {

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
		
	private:
		void updateInput( float );
		void rotateCameraFromCursorPos( vec2 const& );
		void resizeWindow( ui32, ui32 );
		void toggleFullscreen( void );
		void highlightBlock( void );

		void setupVulkanBuffers( void );
		void setupVulkanDescSets( void );
		void setupVulkanPipelines( void );

		ve::VulkanWindow				vulkanWindow;
		ve::VulkanDevice				vulkanDevice;
		ve::VulkanRenderer				vulkanRenderer;
		ve::VulkanDescriptorSetFactory	vulkanSetFactory;
		
		Camera			camera;
		VoxelMap		voxelMap;
		InputHandler	inputHandler;
		ThreadManager	threadManager;
		
		std::unique_ptr<ve::VulkanObject> terrainObject;
		std::unique_ptr<ve::VulkanObject> undergroundObject;
		std::unique_ptr<ve::VulkanObject> skyboxObject;
		std::unique_ptr<ve::VulkanObject> highlightedVoxelObject;

		std::unique_ptr<ve::ViewProjectUniform> matrixUbo;
		std::unique_ptr<ve::MeshUniform>		materialsUbo;

		std::vector<std::unique_ptr<ve::VulkanDescriptorSet>>	uboDescriptorSet;
		std::unique_ptr<ve::VulkanDescriptorSet> 				textureDescriptorSet;

		std::unique_ptr<ve::VulkanPipeline> terrainPipeline;
		std::unique_ptr<ve::VulkanPipeline> skyboxPipeline;
	
		i32	countFramesToUpdate{0};
		bool highlightEnabled{false};
		vec3i highlightedBlock{INT_MAX, INT_MAX, INT_MAX};

		vec3	Vox::screenPointToWorldRay(float mouseX, float mouseY) const;
};

}	// namespace vox
