#pragma once

#include "Camera.hpp"
#include "VulkanObject.hpp"

namespace ve {

struct FrameInfo
{
	int					frameIndex;
	float				frameTime;
	Camera&				camera;
	VkCommandBuffer		commandBuffer;
	VkDescriptorSet		globalDescriptorSet;
	VulkanObject&		gameObject;
	// uint32_t			useTexture{0};
	// bool				rotateModel{false};
};

}	// namespace ve