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
		MatrixUBO( void ) {};
		MatrixUBO( mat4 const& model, mat4 const& view, mat4 const& projection ) :
			model{model},
			view{view},
			projection{projection} {};

		void		updateModel( mat4 const& model ) noexcept { this->model = model; };
		void		updateView( mat4 const& view ) noexcept { this->view = view; };
		void		updateProjection( mat4 const& prj ) noexcept { this->projection = prj; };

		const void*	getData( void ) const noexcept { return static_cast<const void*>(this); };
		size_t		getSize( void ) const noexcept { return sizeof(MatrixUBO); };

	private:
		mat4 model{1.0f};		// NB change into ID	( --> MaterialData (per oggetto)) )
		mat4 view{1.0f};
		mat4 projection{1.0f};
};


class LightUBO
{
	public:
		LightUBO( void ) {};
		LightUBO( vec3 const& lightPos, vec4 const& lightColor, vec3 const& viewPos ) :
			lightPos{lightPos, 1.0f},
			lightColor{lightColor},
			viewPos{viewPos, 1.0f} {};

		void		updateLightPos( vec3 const& lightPos ) noexcept { this->lightPos = vec4{lightPos, 1.0f}; };
		void		updateLightColor( vec3 const& lightColor ) noexcept { this->lightColor = vec4{lightColor, 1.0f}; };
		void		updateViewPos( vec3 const& viewPos ) noexcept { this->viewPos = vec4{viewPos, 1.0f}; };

		const void*	getData( void ) const noexcept { return static_cast<const void*>(this); };
		size_t		getSize( void ) const noexcept { return sizeof(LightUBO); };

	private:
		vec4 lightPos{0.0f};
		vec4 lightColor{1.0f};
		vec4 viewPos{0.0f};
		float ambientStrength{0.6f};		// NB those two should go in another UBO or push const
		float specularStrength{0.3f};
		float _pad[2]; // allineamento std140
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

		std::unique_ptr<ve::VulkanDescriptorSet> uboDescriptorSet;
		std::unique_ptr<ve::VulkanDescriptorSet> textTerrainDescriptorSet;
		std::unique_ptr<ve::VulkanDescriptorSet> textUndergroundDescriptorSet;
		std::unique_ptr<ve::VulkanDescriptorSet> textSkyboxDescriptorSet;

		std::unique_ptr<ve::VulkanPipeline> terrainPipeline;
		std::unique_ptr<ve::VulkanPipeline> skyboxPipeline;

		bool	updateUniforms;
};

}	// namespace vox
