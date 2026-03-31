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

class VulkanRenderSystem
{
	public:

	VulkanRenderSystem(
		VulkanDevice& device,
		VkRenderPass renderPass,
		std::vector<VkDescriptorSetLayout> descriptorSetLayouts,
		char const* vertexShaderFile,
		char const* fragmentShaderFile,
		ModelType modelType = defaultModelType,
		TextureType textureType = TEXTURE_PLAIN
	);
	~VulkanRenderSystem();

	VulkanRenderSystem() = delete;
	VulkanRenderSystem(const VulkanRenderSystem&) = delete;
	VulkanRenderSystem& operator=(const VulkanRenderSystem&) = delete;

	void	renderObject(FrameInfo& frameInfo);

	private:

	void	createPipelineLayout(std::vector<VkDescriptorSetLayout> descriptorSetLayouts);
	void	createPipeline(VkRenderPass renderPass, ModelType modelType);
	void	createPipelineCubemap(VkRenderPass renderPass, ModelType modelType);

	VulkanDevice&	vulkanDevice;

	std::unique_ptr<VulkanPipeline>	vulkanPipeline;
	VkPipelineLayout				pipelineLayout;
	char const*						vertexShaderFile;
	char const*						fragmentShaderFile;
};

}