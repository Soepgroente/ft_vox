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
	// uint32_t	useTexture;
};

VulkanRenderSystem::VulkanRenderSystem(VulkanDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout, char const* vertexShaderFile, char const* fragmentShaderFile) :
	vulkanDevice(device),
	vertexShaderFile(vertexShaderFile),
	fragmentShaderFile(fragmentShaderFile)
{
	createPipelineLayout(globalSetLayout);
	createPipeline(renderPass);
}

VulkanRenderSystem::~VulkanRenderSystem()
{
	vkDestroyPipelineLayout(vulkanDevice.device(), pipelineLayout, nullptr);
}

void	VulkanRenderSystem::createPipelineLayout(VkDescriptorSetLayout globalSetLayout)
{
	VkPushConstantRange	pushConstantRange{};

	pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
	pushConstantRange.offset = 0;
	pushConstantRange.size = sizeof(SimplePushConstantData);

	std::vector<VkDescriptorSetLayout>	descriptorSetLayouts = {globalSetLayout};
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

void	VulkanRenderSystem::renderObject(FrameInfo& frameInfo)
{
	vulkanPipeline->bind(frameInfo.commandBuffer);

	vkCmdBindDescriptorSets(
		frameInfo.commandBuffer,
		VK_PIPELINE_BIND_POINT_GRAPHICS,
		pipelineLayout,
		0, 1, &frameInfo.globalDescriptorSet,
		0, nullptr
	);
	SimplePushConstantData	push{};
	// NB do they have an active role in the rendering system?
	push.modelMatrix = frameInfo.gameObject.transform.matrix4(frameInfo.gameObject.model->getBoundingCenter());
	push.normalMatrix = frameInfo.gameObject.transform.normalMatrix();

	vkCmdPushConstants(
		frameInfo.commandBuffer,
		pipelineLayout,
		VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
		0,
		sizeof(SimplePushConstantData),
		&push
	);
	frameInfo.gameObject.model->bind(frameInfo.commandBuffer);
	frameInfo.gameObject.model->draw(frameInfo.commandBuffer);
}

} // namespace ve