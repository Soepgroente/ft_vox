#pragma once

#include "Camera.hpp"
#include "VulkanDevice.hpp"
// #include "VulkanFrameInfo.hpp"
#include "VulkanModel.hpp"
// #include "VulkanObject.hpp"
// #include "VulkanPipeline.hpp"
#include "VulkanTexture.hpp"

#include <GLFW/glfw3.h>
#include <array>
#include <memory>
#include <vector>


namespace ve {

struct VulkanPipelineConfig
{
	VkPipelineViewportStateCreateInfo		viewportInfo;
	VkPipelineInputAssemblyStateCreateInfo	inputAssemblyInfo;
	VkPipelineRasterizationStateCreateInfo	rasterizationInfo;
	VkPipelineMultisampleStateCreateInfo	multisampleInfo;
	VkPipelineColorBlendAttachmentState		colorBlendAttachment;
	VkPipelineColorBlendStateCreateInfo		colorBlendInfo;
	VkPipelineDepthStencilStateCreateInfo	depthStencilInfo;
	std::vector<VkDynamicState>				dynamicStateEnables;
	VkPipelineDynamicStateCreateInfo		dynamicStateInfo;
	VkPipelineLayout						pipelineLayout = nullptr;
	VkRenderPass							renderPass = nullptr;
	uint32_t								subpass = 0;
};

class VulkanShader
{
	public:
		VulkanShader( VulkanDevice& device, std::string const& shaderPath );
		~VulkanShader( void );
		VulkanShader( VulkanShader const& other ) = delete;
		VulkanShader( VulkanShader&& other );		// NB add move csts in the others classes
		VulkanShader& operator=( VulkanShader const& other ) = delete;

		VkShaderModule	getModule( void ) const noexcept { return this->shaderModule; };

	private:
		std::vector<char>	readFile(std::string const& filePath);
		void				createModule(std::vector<char> const& fileContent);

		VulkanDevice&	vulkanDevice;
		std::string		shaderPath;
		VkShaderModule	shaderModule;
};

class VulkanPipeline
{
	public:
		~VulkanPipeline();
		VulkanPipeline(
			VulkanDevice& device,
			std::vector<VkDescriptorSetLayout> const& descriptorSetLayouts,
			std::string const& vertexShaderFile,
			std::string const& fragmentShaderFile,
			VkRenderPass renderPass,
			ModelType meshType,
			bool hasCubemapsTexture
		);
		VulkanPipeline() = delete;
		VulkanPipeline(VulkanPipeline const&) = delete;
		VulkanPipeline& operator=(VulkanPipeline const&) = delete;

		VkPipelineLayout const&	getPipelineLayout() const noexcept { return pipelineLayout; };
		void					bind(VkCommandBuffer commandBuffer) const noexcept;

		static std::unique_ptr<VulkanPipeline> createPipeline(
			VulkanDevice& device,
			std::vector<VkDescriptorSetLayout> const& descriptorSetLayouts,
			std::string const& vertexShaderFile,
			std::string const& fragmentShaderFile,
			VkRenderPass renderPass,
			ModelType meshType = DEFAULT_MESH_LAYOUT,
			bool hasCubemapsTexture = false
		);

	private:
		void	setupPipelineLayout(std::vector<VkDescriptorSetLayout> const& descriptorSetLayouts);
		void	setupPipeline(std::string const& vertexShaderFile, std::string const& fragmentShaderFile, ModelType meshType, VkRenderPass renderPass);

		std::vector<VkPipelineShaderStageCreateInfo>	getShadersConfig( std::map<VkShaderStageFlagBits,VulkanShader> const& shaders ) const noexcept; 
		VkPipelineVertexInputStateCreateInfo			getVertexBufferConfig( ModelType meshType ) const noexcept;
		VulkanPipelineConfig							getPipelineConfig(VkRenderPass renderPass) const noexcept;

		VulkanDevice&		vulkanDevice;
		bool				hasCubemapsTexture;
		VkPipeline			pipeline;
		VkPipelineLayout	pipelineLayout;
};

}