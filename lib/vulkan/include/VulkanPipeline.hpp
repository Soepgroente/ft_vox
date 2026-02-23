#pragma once

#include <cassert>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include "VulkanDevice.hpp"

namespace ve {

struct PipelineConfigInfo
{
	PipelineConfigInfo() = default;
	PipelineConfigInfo(const PipelineConfigInfo&) = delete;
	PipelineConfigInfo& operator=(const PipelineConfigInfo&) = delete;

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

class VulkanPipeline
{
	public:

	VulkanPipeline(
		VulkanDevice& device,
		const std::string& vertPath,
		const std::string& fragPath,
		const PipelineConfigInfo& PipelineConfigInfo
	);
	~VulkanPipeline();

	VulkanPipeline() = delete;
	VulkanPipeline(const VulkanPipeline&) = delete;
	VulkanPipeline& operator=(const VulkanPipeline&) = delete;

	void	bind(VkCommandBuffer commandBuffer);

	static void	defaultPipelineConfigInfo(PipelineConfigInfo& configInfo);

	private:

	static std::vector<char>	readFile(const std::string& filePath);

	void	createGraphicsPipeline(
		const std::string& vertPath,
		const std::string& fragPath,
		const PipelineConfigInfo& pipelineConfigInfo
	);

	void	createShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule);

	VulkanDevice&	vulkanDevice;
	VkPipeline		graphicsPipeline;
	VkShaderModule	vertexShaderModule;
	VkShaderModule	fragmentShaderModule;
};

}