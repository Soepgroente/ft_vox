#include "VulkanTexture.hpp"

namespace ve {

VulkanTexture::VulkanTexture(const std::string& filePath, VulkanDevice& device) : device(device)
{
	textureImage = VK_NULL_HANDLE;
	textureImageView = VK_NULL_HANDLE;
	textureSampler = VK_NULL_HANDLE;

	imageInfo = loadImage(filePath);
	if (imageInfo.imageData == nullptr)
	{
		throw std::runtime_error("failed to load texture image!");
	}
	imageSize = static_cast<VkDeviceSize>(imageInfo.width) * imageInfo.height * 4;

	info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	info.imageType = VK_IMAGE_TYPE_2D;
	info.extent.width = static_cast<uint32_t>(imageInfo.width);
	info.extent.height = static_cast<uint32_t>(imageInfo.height);
	info.extent.depth = 1;
	info.mipLevels = 1;
	info.arrayLayers = 1;
	info.format = VK_FORMAT_R8G8B8A8_SRGB;
	info.tiling = VK_IMAGE_TILING_OPTIMAL;
	info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	info.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
	info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	info.samples = VK_SAMPLE_COUNT_1_BIT;
	info.flags = 0;

	createTextureImage();
	createTextureImageView();
	createTextureSampler();
}

VulkanTexture::~VulkanTexture()
{
	if (textureImageView != VK_NULL_HANDLE)
	{
		vkDestroyImageView(device.device(), textureImageView, nullptr);
	}
	if (textureSampler != VK_NULL_HANDLE)
	{
		vkDestroySampler(device.device(), textureSampler, nullptr);
	}
	if (textureImage != VK_NULL_HANDLE)
	{
		vkDestroyImage(device.device(), textureImage, nullptr);
	}
	if (textureImageMemory != VK_NULL_HANDLE)
	{
		vkFreeMemory(device.device(), textureImageMemory, nullptr);
	}
}

VulkanTexture::VulkanTexture(VulkanTexture&& other) :
	imageInfo(other.imageInfo),
	imageSize(other.imageSize),
	textureImage(other.textureImage),
	textureImageView(other.textureImageView),
	textureSampler(other.textureSampler),
	info(other.info),
	device(other.device)
{
	other.imageInfo = {};
	other.imageSize = 0;
	other.textureImage = VK_NULL_HANDLE;
	other.textureImageView = VK_NULL_HANDLE;
	other.textureSampler = VK_NULL_HANDLE;
}

VulkanTexture&	VulkanTexture::operator=(VulkanTexture&& other)
{
	if (this != &other)
	{
		imageInfo = other.imageInfo;
		imageSize = other.imageSize;
		textureImage = other.textureImage;
		textureImageView = other.textureImageView;
		textureSampler = other.textureSampler;
		info = other.info;

		other.imageInfo = {};
		other.imageSize = 0;
		other.textureImage = VK_NULL_HANDLE;
		other.textureImageView = VK_NULL_HANDLE;
		other.textureSampler = VK_NULL_HANDLE;
	}
	return *this;
}

void	VulkanTexture::createTextureImage()
{
	VkBuffer		stagingBuffer;
	VkDeviceMemory	stagingBufferMemory;

	device.createBuffer(
		imageSize,
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		stagingBuffer,
		stagingBufferMemory
	);

	void* data;

	vkMapMemory(device.device(), stagingBufferMemory, 0, imageSize, 0, &data);
	memcpy(data, imageInfo.imageData, static_cast<size_t>(imageSize));
	vkUnmapMemory(device.device(), stagingBufferMemory);
	free((const_cast<unsigned char*>(imageInfo.imageData)));
	imageInfo.imageData = nullptr;

	device.createImageWithInfo(
		info,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		textureImage,
		textureImageMemory
	);

	device.transitionImageLayout(
		textureImage,
		info.format,
		VK_IMAGE_LAYOUT_UNDEFINED,
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		1
	);

	device.copyBufferToImage(
		stagingBuffer,
		textureImage,
		static_cast<uint32_t>(imageInfo.width),
		static_cast<uint32_t>(imageInfo.height),
		1
	);
	device.transitionImageLayout(
		textureImage,
		info.format,
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
		1
	);

	vkDestroyBuffer(device.device(), stagingBuffer, nullptr);
	vkFreeMemory(device.device(), stagingBufferMemory, nullptr);
}

void	VulkanTexture::createTextureImageView()
{
	textureImageView = device.createImageView(
		textureImage,
		info.format,
		VK_IMAGE_ASPECT_COLOR_BIT,
		1
	);
}

void	VulkanTexture::createTextureSampler()
{
	VkSamplerCreateInfo	samplerInfo{};

	samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerInfo.magFilter = VK_FILTER_LINEAR;
	samplerInfo.minFilter = VK_FILTER_LINEAR;
	samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.anisotropyEnable = VK_TRUE;
	samplerInfo.maxAnisotropy = device.properties.limits.maxSamplerAnisotropy;
	samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	samplerInfo.unnormalizedCoordinates = VK_FALSE;
	samplerInfo.compareEnable = VK_FALSE;
	samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
	samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	samplerInfo.mipLodBias = 0.0f;
	samplerInfo.minLod = 0.0f;
	samplerInfo.maxLod = 0.0f;

	samplerInfo.maxAnisotropy = 1.0f;

	if (vkCreateSampler(device.device(), &samplerInfo, nullptr, &textureSampler) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create texture sampler!");
	}
}

} // namespace ve