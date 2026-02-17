#pragma once

#include "VulkanObject.hpp"
#include "stb_image.h"

namespace ve {

class VulkanTexture
{
	public:

	VulkanTexture() = delete;
	VulkanTexture(const std::string& filePath, VulkanDevice& device);
	~VulkanTexture();

	VulkanTexture(const VulkanTexture& other) = delete;
	VulkanTexture&	operator=(const VulkanTexture& other) = delete;

	VulkanTexture(VulkanTexture&&);
	VulkanTexture&	operator=(VulkanTexture&&);

	void	createTextureImage();
	void	createTextureImageView();
	void	createTextureSampler();

	const ImageInfo&	getImageInfo() const noexcept { return imageInfo; }
	VkImageView			getImageView() const noexcept { return textureImageView; }
	VkSampler			getSampler() const noexcept { return textureSampler; }

	private:

	ImageInfo		imageInfo;
	VkDeviceSize	imageSize;
	VkImage			textureImage = VK_NULL_HANDLE;
	VkDeviceMemory	textureImageMemory = VK_NULL_HANDLE;
	
	VkImageView		textureImageView = VK_NULL_HANDLE;
	VkSampler		textureSampler = VK_NULL_HANDLE;

	VkImageCreateInfo	info{};

	VulkanDevice&	device;
};

} // namespace ve