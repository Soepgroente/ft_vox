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
	uint32_t	useTexture;
};

VulkanRenderSystem::VulkanRenderSystem(VulkanDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout)
	: vulkanDevice(device)
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
		"build/basic.vert.spv",	// NB store files inside env file?
		"build/basic.frag.spv",
		pipelineConfig
	);
}

void	VulkanRenderSystem::renderObjects(FrameInfo& frameInfo)
{
	vulkanPipeline->bind(frameInfo.commandBuffer);

	vkCmdBindDescriptorSets(
		frameInfo.commandBuffer,
		VK_PIPELINE_BIND_POINT_GRAPHICS,
		pipelineLayout,
		0, 1, &frameInfo.globalDescriptorSet,
		0, nullptr
	);
	for (std::pair<const id_t, VulkanObject>& kv : frameInfo.gameObjects)
	{
		VulkanObject& obj = kv.second;

		if (obj.model == nullptr)
		{
			continue;
		}
		if (frameInfo.rotateModel == true)
		{
			obj.transform.rotation.y = std::fmod(obj.transform.rotation.y + 0.015f, two_pi());
		}
		SimplePushConstantData	push{};
		// default rotation of an object by 55° along x
		// so that the camera looks at it in front instead at it from above
		obj.transform.rotation = vec3{55.0f, 0.0f, 0.0f};
		push.modelMatrix = obj.transform.matrix4(obj.model->getBoundingCenter());
		push.normalMatrix = obj.transform.normalMatrix();
		push.useTexture = frameInfo.useTexture;

		vkCmdPushConstants(
			frameInfo.commandBuffer,
			pipelineLayout,
			VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
			0,
			sizeof(SimplePushConstantData),
			&push
		);
		obj.model->bind(frameInfo.commandBuffer);
		obj.model->draw(frameInfo.commandBuffer);
	}
}

} // namespace ve