#pragma once

#include "Camera.hpp"
#include "VulkanObject.hpp"

namespace ve {

struct FrameInfo
{
	int					frameIndex;
	VkCommandBuffer		commandBuffer;
	VkDescriptorSet		globalDescriptorSet;
	VulkanObject		gameObject;
};

}	// namespace ve