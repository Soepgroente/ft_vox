#pragma once

#include "Vulkan.hpp"
#include "Vectors.hpp"

#include "Camera.hpp"
#include "Config.hpp"
#include "World.hpp"
#include "InputHandler.hpp"
#include "ThreadManager.hpp"
#include "VoxelMap.hpp"

#include <cstdint>
#include <thread>

namespace vox {

using ui32 = uint32_t;
using i32 = int32_t;


class ViewProjectUBO
{
	public:
		ViewProjectUBO( void ) = delete;
		ViewProjectUBO( mat4 const& view, mat4 const& projection ) :
			view{view},
			projection{projection} {};

		void	updateView( mat4 const& view ) noexcept { this->view = view; };
		void	updateProjection( mat4 const& prj ) noexcept { this->projection = prj; };

		const void*	getData( void ) const noexcept { return static_cast<const void*>(this); };

	private:
		mat4 view{1.0f};
		mat4 projection{1.0f};
};


class LightUBO
{
	public:
		LightUBO( void ) = delete;
		LightUBO( vec3 const& lightPos, vec4 const& lightColor, vec3 const& viewPos ) :
			lightPos{lightPos, 1.0f},
			lightColor{lightColor},
			viewPos{viewPos, 1.0f} {};

		void	updateLightPos( vec3 const& lightPos ) noexcept { this->lightPos = vec4{lightPos, 1.0f}; };
		void	updateLightColor( vec3 const& lightColor ) noexcept { this->lightColor = vec4{lightColor, 1.0f}; };
		void	updateViewPos( vec3 const& viewPos ) noexcept { this->viewPos = vec4{viewPos, 1.0f}; };

		const void*	getData( void ) const noexcept { return static_cast<const void*>(this); };

	private:
		vec4 lightPos{0.0f};
		vec4 lightColor{1.0f};
		vec4 viewPos{0.0f};
};


class MeshData
{
	public:
		MeshData( void ) = delete;
		MeshData( mat4 const& modelMatrix, mat4 const& normalMatrix ) :
			modelMatrix{modelMatrix},
			normalMatrix{normalMatrix} {};
		MeshData( MeshData const& other ) = default;
		MeshData( MeshData&& other ) = default;
		MeshData& operator=( MeshData const& other ) = default;
		MeshData& operator=( MeshData&& other ) = default;

		void	updateModelMatrix( mat4 const& modelMatrix ) noexcept { this->modelMatrix = modelMatrix; };
		void	updateNormalMatrix( mat4 const& normalMatrix ) noexcept { this->normalMatrix = normalMatrix; };
		void	updateMaterial( ve::MeshMaterial const& material ) noexcept { this->material = material; };

		const void*	getData( void ) const noexcept { return static_cast<const void*>(this); };

	private:
		mat4	modelMatrix{1.0f};
		mat4	normalMatrix{1.0f};
		ve::MeshMaterial	material{};
};

static_assert(sizeof(MeshData) == 192);
static_assert(sizeof(MeshData) == 2 * sizeof(mat4) + sizeof(ve::MeshMaterial));


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

		static std::vector<std::thread>	workerThreads;

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

		std::unique_ptr<ve::VulkanDescriptorSet> uboDescriptorSet;
		std::unique_ptr<ve::VulkanDescriptorSet> textTerrainDescriptorSet;
		std::unique_ptr<ve::VulkanDescriptorSet> textUndergroundDescriptorSet;
		std::unique_ptr<ve::VulkanDescriptorSet> textSkyboxDescriptorSet;

		std::unique_ptr<ve::VulkanPipeline> terrainPipeline;
		std::unique_ptr<ve::VulkanPipeline> skyboxPipeline;

		bool	updateUniforms;
};

}	// namespace vox
