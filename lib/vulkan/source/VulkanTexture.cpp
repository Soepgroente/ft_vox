#include "VulkanTexture.hpp"
#include <iostream>

namespace ve {

VulkanTexture::VulkanTexture(const std::string& filePath, VulkanDevice& device, TextureType type) : 
	device(device), type(type)
{
	textureImage = VK_NULL_HANDLE;
	textureImageView = VK_NULL_HANDLE;
	textureSampler = VK_NULL_HANDLE;

	imageInfo = loadImage(filePath);
	if (imageInfo.imageData == nullptr)
	{
		throw std::runtime_error("failed to load texture image!");
	}

	info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	info.imageType = VK_IMAGE_TYPE_2D;
	info.extent.depth = 1;
	info.mipLevels = 1;
	info.format = VK_FORMAT_R8G8B8A8_SRGB;
	info.tiling = VK_IMAGE_TILING_OPTIMAL;
	info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	info.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
	info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	info.samples = VK_SAMPLE_COUNT_1_BIT;
	if (type == TEXTURE_PLAIN) {
		info.arrayLayers = 1;
		info.flags = 0;
		info.extent.width = static_cast<uint32_t>(imageInfo.width);
		info.extent.height = static_cast<uint32_t>(imageInfo.height);
		imageSize = static_cast<VkDeviceSize>(imageInfo.width) * imageInfo.height * 4;
	} else if (type == TEXTURE_CUBEMAP) {
		info.arrayLayers = 6;
		info.flags = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
		uint32_t faceSize = imageInfo.width / 4;
		info.extent.width = static_cast<uint32_t>(faceSize);
		info.extent.height = static_cast<uint32_t>(faceSize);
		imageSize = faceSize * faceSize * 6 * 4;	// 6 are the faces per picture, 4 is the size in pixel of every pixel
	}

	createTextureImage();
	createTextureImageView();
	createTextureSampler();

	// NB do that:
	// 1. carica l'immagine totale da disco
	// int totalWidth, totalHeight, channels;
	// stbi_uc* pixels = stbi_load(path, &totalWidth, &totalHeight, &channels, STBI_rgb_alpha);

	// int faceWidth  = totalWidth / 4;
	// int faceHeight = totalHeight / 3;
	// VkDeviceSize faceSize  = faceWidth * faceHeight * 4;
	// VkDeviceSize totalSize = faceSize * 6;

	// // 2. crea lo staging buffer di totalSize
	// // ...
	// void* stagingPtr;
	// vkMapMemory(device, stagingMemory, 0, totalSize, 0, &stagingPtr);

	// // 3. ← QUI — estrai le 6 facce e copiale nello staging buffer
	// for (int face = 0; face < 6; face++) {
	// 	auto [col, row] = offsets[face];
	// 	int srcX = col * faceWidth;
	// 	int srcY = row * faceHeight;

	// 	for (int y = 0; y < faceHeight; y++) {
	// 		memcpy(
	// 			(uint8_t*)stagingPtr + face * faceSize + y * faceWidth * 4,
	// 			pixels + (srcY + y) * totalWidth * 4 + srcX * 4,
	// 			faceWidth * 4
	// 		);
	// 	}
	// }

	// vkUnmapMemory(device, stagingMemory);
	// stbi_image_free(pixels);

	// // 4. crea VkImage con arrayLayers=6, VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT
	// // ...

	// // 5. transizione layout UNDEFINED → TRANSFER_DST_OPTIMAL
	// // ...

	// // 6. vkCmdCopyBufferToImage con le 6 regions
	// // ...

	// // 7. transizione layout TRANSFER_DST_OPTIMAL → SHADER_READ_ONLY_OPTIMAL
	// // ...

	// // 8. crea VkImageView con viewType=VK_IMAGE_VIEW_TYPE_CUBE
	// // ...
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
	textureImageMemory(other.textureImageMemory),
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
	other.textureImageMemory = VK_NULL_HANDLE;
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
		other.textureImageMemory = VK_NULL_HANDLE;
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
		(this->type == TEXTURE_PLAIN) ? 1 : 6
	);

	device.copyBufferToImage(
		stagingBuffer,
		textureImage,
		static_cast<uint32_t>(imageInfo.width),
		static_cast<uint32_t>(imageInfo.height),
		1,
		type
	);
	device.transitionImageLayout(
		textureImage,
		info.format,
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
		(this->type == TEXTURE_PLAIN) ? 1 : 6
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
		info.arrayLayers,
		type
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
	// NB for cubemaps
	// samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
	// samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
	// samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
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