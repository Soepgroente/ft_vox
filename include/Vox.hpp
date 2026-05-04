#pragma once

#include "Vulkan.hpp"
#include "Camera.hpp"
#include "Config.hpp"
#include "InputHandler.hpp"
#include "ThreadManager.hpp"
#include "VoxelMap.hpp"

#include <array>
#include <memory>
#include <vector>

namespace vox {

using ui32 = uint32_t;
using i32 = int32_t;


class MatrixUBO
{
	public:
		MatrixUBO(Camera& camera) :
			model{mat4::idMat()},
			view{camera.getViewMatrix()},
			projection{camera.getProjectionMatrix()} {};

		const void*	getData( void ) const noexcept { return static_cast<const void*>(this); };
		size_t		getSize( void ) const noexcept { return sizeof(MatrixUBO); };

	private:
		mat4 model{1.0f};
		mat4 view{1.0f};
		mat4 projection{1.0f};
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

	private:
		std::unique_ptr<ve::VulkanModel> createSkyboxModel( void );

		ve::VulkanWindow				vulkanWindow;
		ve::VulkanDevice				vulkanDevice;
		ve::VulkanRenderer				vulkanRenderer;
		ve::VulkanDescriptorSetFactory	vulkanSetFactory;

		Camera			camera;
		VoxelMap		voxelMap;
		InputHandler	inputHandler;
		ThreadManager	threadManager;

		std::unique_ptr<ve::VulkanModel> terrainModel;
		std::unique_ptr<ve::VulkanModel> undergroundModel;
		std::unique_ptr<ve::VulkanModel> skyBoxModel;

		std::unique_ptr<ve::VulkanDescriptorSet> matrixDescriptorSet;
		std::unique_ptr<ve::VulkanDescriptorSet> textTerrainDescriptorSet;
		std::unique_ptr<ve::VulkanDescriptorSet> textUndergroundDescriptorSet;
		std::unique_ptr<ve::VulkanDescriptorSet> textSkyboxDescriptorSet;

		std::unique_ptr<ve::VulkanPipeline> terrainPipeline;
		std::unique_ptr<ve::VulkanPipeline> skyboxPipeline;

		bool	updateMatrixUbo;
};

}	// namespace vox
