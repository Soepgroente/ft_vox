#pragma once

#include "Vulkan.hpp"

#include "Camera.hpp"
#include "World.hpp"
#include "InputHandler.hpp"
#include "ThreadManager.hpp"
#include "VoxelMap.hpp"

#include <cstdint>
#include <thread>

namespace vox {

using ui32 = uint32_t;
using i32 = int32_t;


// Vulkan layout veriables for shaders in 16b so each member size has to be 16 or multiple or padded
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
		mat4 view;
		mat4 projection;
};

// Vulkan layout veriables for shaders in 16b so each member size has to be 16 or multiple or padded
class LightUBO
{
	public:
		LightUBO( void ) = delete;
		LightUBO( vec3 const& lightPos, vec3 const& viewPos, vec3 const& ambientColor, vec3 const& diffuseColor, vec3 const& specularColor ) :
			lightPos{lightPos, 1.0f},
			viewPos{viewPos, 1.0f},			// NB instead of updating the viewPos every time, it is possible to apply view trasnformation to normals, but view matrix has to change 
			lightAmbientColor{ambientColor, 1.0f},
			lightColor{diffuseColor, 1.0f},
			lightSpecularColor{specularColor, 1.0f} {};

		void	updateLightPos( vec3 const& lightPos ) noexcept { this->lightPos = vec4{lightPos, 1.0f}; };
		void	updateViewPos( vec3 const& viewPos ) noexcept { this->viewPos = vec4{viewPos, 1.0f}; };

		const void*	getData( void ) const noexcept { return static_cast<const void*>(this); };

	private:
		vec4 	lightPos;
		vec4 	viewPos;
		vec4	lightAmbientColor;
		vec4	lightColor;
		vec4	lightSpecularColor;
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
		mat4				modelMatrix{1.0f};
		mat4				normalMatrix{1.0f};
		ve::MeshMaterial	material{};
};

// vulkan push_constant max size should be 128 or 256 b
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
		std::shared_ptr<ve::VulkanModel> createVoxelMesh( vec3 const& = vec3{-0.5f, -0.5f, -0.5f} );

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
		std::unique_ptr<ve::VulkanObject> sunObject;

		std::unique_ptr<ve::VulkanDescriptorSet> uboDescriptorSet;
		std::unique_ptr<ve::VulkanDescriptorSet> textTerrainDescriptorSet;
		std::unique_ptr<ve::VulkanDescriptorSet> textUndergroundDescriptorSet;
		std::unique_ptr<ve::VulkanDescriptorSet> textSkyboxDescriptorSet;

		std::unique_ptr<ve::VulkanPipeline> terrainPipeline;
		std::unique_ptr<ve::VulkanPipeline> skyboxPipeline;
		std::unique_ptr<ve::VulkanPipeline> sunPipeline;

		bool	updateUniforms;
};

}	// namespace vox
