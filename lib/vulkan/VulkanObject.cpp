#include "VulkanObject.hpp"
#include "stb_image.h"
#include <iostream>

namespace ve {

id_t	VulkanObject::currentID = 0;

mat4	TransformComponent::matrix4() const noexcept
{
	const float c1 = std::cos(rotation.y);
	const float s1 = std::sin(rotation.y);
	const float c2 = std::cos(rotation.x);
	const float s2 = std::sin(rotation.x);
	const float c3 = std::cos(rotation.z);
	const float s3 = std::sin(rotation.z);

	return mat4(
		{
			scale.x * (c1 * c3 + s1 * s2 * s3),
			scale.x * (c2 * s3),
			scale.x * (c1 * s2 * s3 - c3 * s1),
			0.0f
		},
		{
			scale.y * (c3 * s1 * s2 - c1 * s3),
			scale.y * (c2 * c3),
			scale.y * (c1 * c3 * s2 + s1 * s3),
			0.0f,
		},
		{
			scale.z * (c2 * s1),
			scale.z * (-s2),
			scale.z * (c1 * c2),
			0.0f,
		},
		{
			translation.x,
			translation.y,
			translation.z,
			1.0f
		}
	);
}

mat4	TransformComponent::matrix4(const vec3& rotationCenter) const noexcept
{
	mat4 translateToOrigin = mat4(1.0f).translate(rotationCenter.inverted());
	mat4 translateBack = mat4(1.0f).translate(rotationCenter);
	mat4 finalTranslate = mat4(1.0f).translate(translation);

	const float c1 = std::cos(rotation.y);
	const float s1 = std::sin(rotation.y);
	const float c2 = std::cos(rotation.x);
	const float s2 = std::sin(rotation.x);
	const float c3 = std::cos(rotation.z);
	const float s3 = std::sin(rotation.z);

	mat4 rotScale = mat4
	{
		{
			scale.x * (c1 * c3 + s1 * s2 * s3),
			scale.x * (c2 * s3),
			scale.x * (c1 * s2 * s3 - c3 * s1),
			0.0f,
		},
		{
			scale.y * (c3 * s1 * s2 - c1 * s3),
			scale.y * (c2 * c3),
			scale.y * (c1 * c3 * s2 + s1 * s3),
			0.0f,
		},
		{
			scale.z * (c2 * s1),
			scale.z * (-s2),
			scale.z * (c1 * c2),
			0.0f,
		},
		{0.0f, 0.0f, 0.0f, 1.0f}
	};

	return finalTranslate * translateBack * rotScale * translateToOrigin;
}

mat3	TransformComponent::normalMatrix() const noexcept
{
	const float c1 = std::cos(rotation.y);
	const float s1 = std::sin(rotation.y);
	const float c2 = std::cos(rotation.x);
	const float s2 = std::sin(rotation.x);
	const float c3 = std::cos(rotation.z);
	const float s3 = std::sin(rotation.z);
	const vec3	invScale = 1.0f / scale;

	return mat3
	{
		{
			invScale.x * (c1 * c3 + s1 * s2 * s3),
			invScale.x * (c2 * s3),
			invScale.x * (c1 * s2 * s3 - c3 * s1),
		},
		{
			invScale.y * (c3 * s1 * s2 - c1 * s3),
			invScale.y * (c2 * c3),
			invScale.y * (c1 * c3 * s2 + s1 * s3),
		},
		{
			invScale.z * (c2 * s1),
			invScale.z * (-s2),
			invScale.z * (c1 * c2),
		},
	};
}

VulkanObject::VulkanObject(id_t objID) : id(objID)
{
}

VulkanObject::~VulkanObject()
{
}

ImageInfo	loadImage(const std::string& imagePath)
{
	ImageInfo	imageInfo = {};

	imageInfo.imageData = stbi_load(imagePath.c_str(), &imageInfo.width, &imageInfo.height, &imageInfo.channels, STBI_rgb_alpha);
	if (imageInfo.imageData == nullptr)
	{
		throw std::runtime_error("Failed to load image: " + imagePath);
	}
	std::cout << "Loaded image: " << imagePath << " (" << imageInfo.width << "x" << imageInfo.height << ", " << imageInfo.channels << " channels)" << std::endl;
	return imageInfo;
}

}	// namespace ve