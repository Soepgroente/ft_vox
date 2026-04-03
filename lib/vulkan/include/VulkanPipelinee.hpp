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
		VulkanShader& operator=( VulkanShader const& other ) = delete;

		VkShaderModule	getModule( void ) const noexcept { return this->shaderModule; };

	private:
		std::vector<char>	readFile(std::string const& filePath);
		void				createModule(std::vector<char> const& fileContent);

		VulkanDevice&	vulkanDevice;
		std::string		shaderPath;
		VkShaderModule	shaderModule;
};

class VulkanPipelinee
{
	public:
		VulkanPipelinee(
			VulkanDevice& device,
			VkRenderPass renderPass,
			std::vector<VkDescriptorSetLayout> const& descriptorSetLayouts,
			std::string const& vertexShaderFile,
			std::string const& fragmentShaderFile,
			ModelType modelType = defaultModelType,
			TextureType textureType = TEXTURE_PLAIN
		);
		~VulkanPipelinee();

		VulkanPipelinee() = delete;
		VulkanPipelinee(const VulkanPipelinee&) = delete;
		VulkanPipelinee& operator=(const VulkanPipelinee&) = delete;

		const VkPipelineLayout&	getPipelineLayout() const noexcept {return pipelineLayout; };
		void					bind(VkCommandBuffer commandBuffer);

	private:
		void	createPipelineLayout(std::vector<VkDescriptorSetLayout> const& descriptorSetLayouts);
		void	createPipeline(
			VkRenderPass renderPass,
			std::string const& vertexShaderFile,
			std::string const& fragmentShaderFile,
			ModelType modelType,
			TextureType textureType
		);

		VulkanPipelineConfig	defaultPipelineConfigInfo(VkRenderPass renderPass, TextureType textureType) const noexcept;

		VulkanDevice&		vulkanDevice;
		VkPipeline			pipeline;
		VkPipelineLayout	pipelineLayout;
};

}