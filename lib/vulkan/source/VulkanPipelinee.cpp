#include "VulkanRenderSystem.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <chrono>
#include <random>

namespace ve {

struct SimplePushConstantData
{
	mat4		modelMatrix{1.0f};
	mat4		normalMatrix{1.0f};
};

VulkanRenderSystem::VulkanRenderSystem(
		VulkanDevice& device,
		VkRenderPass renderPass,
		std::vector<VkDescriptorSetLayout> descriptorSetLayouts,
		char const* vertexShaderFile,
		char const* fragmentShaderFile,
		ModelType modelType,
		TextureType textureType
	) :
	vulkanDevice(device),
	vertexShaderFile(vertexShaderFile),
	fragmentShaderFile(fragmentShaderFile)
{
	createPipelineLayout(descriptorSetLayouts);
	switch (textureType) {
		case TEXTURE_PLAIN:
			createPipeline(renderPass, modelType);
			break;
		case TEXTURE_CUBEMAP:
			createPipelineCubemap(renderPass, modelType);
			break;
		// case default:
		// 	break;
	}
}

VulkanRenderSystem::~VulkanRenderSystem()
{
	vkDestroyPipelineLayout(vulkanDevice.device(), pipelineLayout, nullptr);
}

void	VulkanRenderSystem::createPipelineLayout(std::vector<VkDescriptorSetLayout> descriptorSetLayouts)
{
	VkPipelineLayoutCreateInfo	pipelineLayoutInfo{};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
	pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();

	if (vkCreatePipelineLayout(vulkanDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create pipeline layout!");
	}
}

void	VulkanRenderSystem::createPipeline(VkRenderPass renderPass, ModelType modelType)
{
	assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

	PipelineConfigInfo	pipelineConfig{};
	VulkanPipeline::defaultPipelineConfigInfo(pipelineConfig);

	pipelineConfig.pipelineLayout = pipelineLayout;
	pipelineConfig.renderPass = renderPass;

	vulkanPipeline = std::make_unique<VulkanPipeline>(
		vulkanDevice,
		this->vertexShaderFile,
		this->fragmentShaderFile,
		pipelineConfig,
		modelType
	);
}

void	VulkanRenderSystem::createPipelineCubemap(VkRenderPass renderPass, ModelType modelType)
{
	assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

	PipelineConfigInfo	pipelineConfig{};
	VulkanPipeline::defaultPipelineConfigInfo(pipelineConfig);
	// cubemap config
	pipelineConfig.depthStencilInfo.depthWriteEnable = VK_FALSE;
	pipelineConfig.depthStencilInfo.depthTestEnable = VK_TRUE;
	pipelineConfig.depthStencilInfo.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
	pipelineConfig.rasterizationInfo.cullMode = VK_CULL_MODE_NONE;

	pipelineConfig.pipelineLayout = pipelineLayout;
	pipelineConfig.renderPass = renderPass;

	vulkanPipeline = std::make_unique<VulkanPipeline>(
		vulkanDevice,
		this->vertexShaderFile,
		this->fragmentShaderFile,
		pipelineConfig,
		modelType
	);
}

} // namespace ve