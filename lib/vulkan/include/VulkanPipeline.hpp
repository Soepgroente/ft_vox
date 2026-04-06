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
	VkPipelineVertexInputStateCreateInfo 	vertexInputInfo;
	VkPipelineViewportStateCreateInfo		viewportInfo;
	VkPipelineInputAssemblyStateCreateInfo	inputAssemblyInfo;
	VkPipelineRasterizationStateCreateInfo	rasterizationInfo;
	VkPipelineMultisampleStateCreateInfo	multisampleInfo;
	VkPipelineColorBlendAttachmentState		colorBlendAttachment;
	VkPipelineColorBlendStateCreateInfo		colorBlendInfo;
	VkPipelineDepthStencilStateCreateInfo	depthStencilInfo;
	std::vector<VkDynamicState>				dynamicStateEnables;
	VkPipelineDynamicStateCreateInfo		dynamicStateInfo;

	std::vector<VkVertexInputBindingDescription>	bindingVboConfig;
	std::vector<VkVertexInputAttributeDescription>	attributeVboConfig;
	std::vector<VkPipelineShaderStageCreateInfo>	shadersConfig;
};

class VulkanShader
{
	public:
		VulkanShader( VulkanDevice& device, VkShaderStageFlagBits shaderStageFlag, std::string const& shaderPath );
		~VulkanShader( void );
		VulkanShader( VulkanShader const& other ) = delete;
		VulkanShader( VulkanShader&& other );		// NB add move csts in the others classes
		VulkanShader& operator=( VulkanShader const& other ) = delete;

		VkShaderModule			getModule( void ) const noexcept { return this->shaderModule; };
		VkShaderStageFlagBits	getStageFlag( void ) const noexcept { return this->shaderStageFlag; };

	private:
		void	createModule(std::vector<char> const& fileContent);

		VulkanDevice&			vulkanDevice;
		VkShaderStageFlagBits	shaderStageFlag;
		std::string				shaderPath;
		VkShaderModule			shaderModule;
};

class VulkanPipeline
{
	public:
		~VulkanPipeline();
		VulkanPipeline(
			VulkanDevice& device,
			std::vector<VkDescriptorSetLayout> const& descriptorSetLayouts,
			VkRenderPass renderPass,
			std::string const& vertexShaderFile,
			std::string const& fragmentShaderFile,
			VulkanModel const& mesh,
			bool hasCubemapsTexture
		);
		VulkanPipeline() = delete;
		VulkanPipeline(VulkanPipeline const&) = delete;
		VulkanPipeline& operator=(VulkanPipeline const&) = delete;

		VkPipelineLayout	getPipelineLayout() const noexcept { return pipelineLayout; };
		void				bind(VkCommandBuffer commandBuffer) const noexcept;

		static std::unique_ptr<VulkanPipeline> createPipeline(
			VulkanDevice& device,
			std::vector<VkDescriptorSetLayout> const& descriptorSetLayouts,
			VkRenderPass renderPass,
			std::string const& vertexShaderFile,
			std::string const& fragmentShaderFile,
			VulkanModel const& mesh,
			bool hasCubemapsTexture = false
		);

	private:
		void					setupPipelineLayout( std::vector<VkDescriptorSetLayout> const& descriptorSetLayouts );
		void					setupPipeline( std::string const& vertexShaderFile, std::string const& fragmentShaderFile, VulkanModel const& mesh, bool hasCubemapsTexture, VkRenderPass renderPass );
		VulkanPipelineConfig	getPipelineConfig( std::vector<VulkanShader> const& shaders, VulkanModel const& mesh, bool hasCubemapsTexture ) const noexcept;

		VulkanDevice&		vulkanDevice;
		VkPipelineLayout	pipelineLayout;
		VkPipeline			pipeline;
};

}