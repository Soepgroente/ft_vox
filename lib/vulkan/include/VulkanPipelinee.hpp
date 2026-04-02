#pragma once

#include "Camera.hpp"
#include "VulkanDevice.hpp"
#include "VulkanFrameInfo.hpp"
#include "VulkanModel.hpp"
#include "VulkanObject.hpp"
#include "VulkanPipeline.hpp"
#include "VulkanTexture.hpp"

#include <GLFW/glfw3.h>
#include <array>
#include <memory>
#include <vector>

namespace ve {

class VulkanPipelinee
{
	public:

	VulkanPipelinee(
		VulkanDevice& device,
		VkRenderPass renderPass,
		std::vector<VkDescriptorSetLayout> descriptorSetLayouts,
		PipelineConfigInfo& PipelineConfigInfo,
		char const* vertexShaderFile,
		char const* fragmentShaderFile,
		ModelType modelType = defaultModelType,
		TextureType textureType = TEXTURE_PLAIN
	);
	~VulkanPipelinee();

	VulkanPipelinee() = delete;
	VulkanPipelinee(const VulkanPipelinee&) = delete;
	VulkanPipelinee& operator=(const VulkanPipelinee&) = delete;

	void	bind(VkCommandBuffer commandBuffer);
	static void	defaultPipelineConfigInfo(PipelineConfigInfo& configInfo);
	void	renderObject(FrameInfo& frameInfo);

	private:

	static std::vector<char>	readFile(const std::string& filePath);

	void	createGraphicsPipeline(
		const std::string& vertPath,
		const std::string& fragPath,
		const PipelineConfigInfo& pipelineConfigInfo,
		ModelType modelType
	);

	void	createShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule);

	void	createPipelineLayout(std::vector<VkDescriptorSetLayout> descriptorSetLayouts);
	void	createPipeline(VkRenderPass renderPass, ModelType modelType);
	void	createPipelineCubemap(VkRenderPass renderPass, ModelType modelType);

	VulkanDevice&	vulkanDevice;
	VkPipeline		graphicsPipeline;
	VkShaderModule	vertexShaderModule;
	VkShaderModule	fragmentShaderModule;

	std::unique_ptr<VulkanPipeline>	vulkanPipeline;
	VkPipelineLayout				pipelineLayout;
	char const*						vertexShaderFile;
	char const*						fragmentShaderFile;
};

}