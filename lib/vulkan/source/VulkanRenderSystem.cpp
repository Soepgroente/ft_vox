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
		TextureType type
	) :
	vulkanDevice(device),
	vertexShaderFile(vertexShaderFile),
	fragmentShaderFile(fragmentShaderFile)
{
	createPipelineLayout(descriptorSetLayouts);
	switch (type) {
		case TEXTURE_PLAIN:
			createPipeline(renderPass);
			break;
		case TEXTURE_CUBEMAP:
			createPipelineCubemap(renderPass);
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
	VkPushConstantRange	pushConstantRange{};		// push constants may not be necessary for every pipeline
	pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
	pushConstantRange.offset = 0;
	pushConstantRange.size = sizeof(SimplePushConstantData);

	VkPipelineLayoutCreateInfo	pipelineLayoutInfo{};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
	pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
	pipelineLayoutInfo.pushConstantRangeCount = 1;
	pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

	if (vkCreatePipelineLayout(vulkanDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create pipeline layout!");
	}
}

void	VulkanRenderSystem::createPipeline(VkRenderPass renderPass)
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
		pipelineConfig
	);
}

void	VulkanRenderSystem::createPipelineCubemap(VkRenderPass renderPass)
{
	assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

	PipelineConfigInfo	pipelineConfig{};

	VulkanPipeline::defaultPipelineConfigInfo(pipelineConfig);
	// cubemap config
	pipelineConfig.depthStencilInfo.depthWriteEnable = VK_FALSE;
	pipelineConfig.depthStencilInfo.depthTestEnable  = VK_TRUE;
	pipelineConfig.depthStencilInfo.depthCompareOp   = VK_COMPARE_OP_LESS_OR_EQUAL;
	pipelineConfig.rasterizationInfo.cullMode = VK_CULL_MODE_NONE;

	pipelineConfig.pipelineLayout = pipelineLayout;
	pipelineConfig.renderPass = renderPass;

	vulkanPipeline = std::make_unique<VulkanPipeline>(
		vulkanDevice,
		this->vertexShaderFile,
		this->fragmentShaderFile,
		pipelineConfig
	);
}
void	VulkanRenderSystem::renderObject(FrameInfo& frameInfo)
{
	if (frameInfo.gameObject.model == nullptr)
	{
		return;
	}

	vkCmdBindDescriptorSets(
		*frameInfo.commandBuffer,
		VK_PIPELINE_BIND_POINT_GRAPHICS,
		pipelineLayout,
		0, 1, &frameInfo.globalDescriptorSet,
		0, nullptr
	);
	vulkanPipeline->bind(*frameInfo.commandBuffer);

	SimplePushConstantData	push{};		// NB maybe not necessary
	push.modelMatrix = frameInfo.gameObject.transform.matrix4(frameInfo.gameObject.model->getBoundingCenter());
	push.normalMatrix = frameInfo.gameObject.transform.normalMatrix();

	vkCmdPushConstants(
		*frameInfo.commandBuffer,
		pipelineLayout,
		VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
		0,
		sizeof(SimplePushConstantData),
		&push
	);
	
	frameInfo.gameObject.model->bind(*frameInfo.commandBuffer);
	frameInfo.gameObject.model->draw(*frameInfo.commandBuffer);
}

} // namespace ve