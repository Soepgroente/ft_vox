#include "VulkanObject.hpp"
#include "stb_image.h"

#include <iostream>
#include <cassert>


namespace ve {

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
			0.0f
		},
		{
			scale.z * (c2 * s1),
			scale.z * (-s2),
			scale.z * (c1 * c2),
			0.0f
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
	mat4 translateToOrigin = mat4(1.0f).translate(rotationCenter.inverted()).transpose();
	mat4 translateBack = mat4(1.0f).translate(rotationCenter).transpose();
	mat4 finalTranslate = mat4(1.0f).translate(translation).transpose();

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


uint32_t	VulkanObject::currentID = 0U;

void VulkanObject::rotate( vec3 const& axis, float angle ) noexcept
{
	if (this->transformationApplied == false)
		this->transformationApplied = true;

	this->transform.rotation = quat::product(quat(angle, axis), transform.rotation);	// second factor is applied first, first as last
	this->transform.rotation.normalize();
}

void VulkanObject::translate( vec3 const& translation ) noexcept
{
	if (this->transformationApplied == false)
		this->transformationApplied = true;

	this->transform.translation += translation;
}

void VulkanObject::scale( vec3 const& scale ) noexcept
{
	if (this->transformationApplied == false)
		this->transformationApplied = true;

	if (this->uniformScale == true and (scale.x != scale.y or scale.y != scale.z))
		this->uniformScale = false;

	this->transform.scale.x *= scale.x;
	this->transform.scale.y *= scale.y;
	this->transform.scale.z *= scale.z;
}

void VulkanObject::scale( float scale ) noexcept
{
	if (this->transformationApplied == false)
		this->transformationApplied = true;

	this->transform.scale.x *= scale;
	this->transform.scale.y *= scale;
	this->transform.scale.z *= scale;
}

void VulkanObject::bindBuffer(VkCommandBuffer commandBuffer) const
{
	assert(this->model != nullptr && "Model not set");
	this->model->bindBuffer(commandBuffer);
};

void VulkanObject::draw(VkCommandBuffer commandBuffer) const
{
	assert(this->model != nullptr && "Model not set");
	this->model->draw(commandBuffer);
};

std::shared_ptr<VulkanModel>	VulkanObject::getModel() const
{
	assert(this->model != nullptr && "Model not set");
	return this->model;
};

// return matrix in column major
mat4 VulkanObject::getModelMatrix() const noexcept
{
	// model = T × R × S
	if (this->transformationApplied == false)
	{
		return mat4::idMat();
	}

	mat4 rotation = this->transform.rotation.getMatrix();

	return mat4(
		{
			this->transform.scale.x * rotation[0][0],
			this->transform.scale.x * rotation[0][1],
			this->transform.scale.x * rotation[0][2],
			0.0f
		},
		{
			this->transform.scale.y * rotation[1][0],
			this->transform.scale.y * rotation[1][1],
			this->transform.scale.y * rotation[1][2],
			0.0f
		},
		{
			this->transform.scale.z * rotation[2][0],
			this->transform.scale.z * rotation[2][1],
			this->transform.scale.z * rotation[2][2],
			0.0f
		},
		{
			this->transform.translation.x,
			this->transform.translation.y,
			this->transform.translation.z,
			1.0f
		}
	);
}

// return matrix in column major
mat4 VulkanObject::getNormalMatrix() const noexcept
{
	// (model = T × R × S)
	// normalMatrix = (Model⁻¹)ᵀ = ((T × R × S)⁻¹)ᵀ = (S⁻¹ * R⁻¹ * T⁻¹)ᵀ = (T⁻¹)ᵀ * (R⁻¹)ᵀ * (S⁻¹)ᵀ but:
	// Transpose matrix: since it moves points, doesn't affect the normal so can be removed
	// Rotation matrix: is orthogonal so  R⁻¹ = Rᵀ --> (R⁻¹)ᵀ = (Rᵀ)ᵀ = R
	// Scale matrix: is diagonal (and therefore symmetric) so Sᵀ = S --> (S⁻¹)ᵀ = S⁻¹ also the inverse of a diagonal matrix
	//		is a matrix which elements are the inverse of the elements of the original matrix, finally, if the scaling is uniform
	//		i.e scale.x = scale.y = scale.z the whole matrix can be ignored
	// therefore: (Model⁻¹)ᵀ = R * [S⁻¹]
	if (this->transformationApplied == false)
	{
		return mat4::idMat();
	}

	mat4 normal = this->transform.rotation.getMatrix();

	if (this->uniformScale == false)
	{
		const float idx = 1.0f / this->transform.scale.x;
		const float idy = 1.0f / this->transform.scale.y;
		const float idz = 1.0f / this->transform.scale.z;

		normal[0][0] *= idx;
		normal[0][1] *= idx;
		normal[0][2] *= idx;
		normal[1][0] *= idy;
		normal[1][1] *= idy;
		normal[1][2] *= idy;
		normal[2][0] *= idz;
		normal[2][1] *= idz;
		normal[2][2] *= idz;
	}

	return normal;
}

mat4 VulkanObject::getNormalViewMatrix(const mat4& viewNoTranslation) const noexcept
{
	// (model = T × R × S)
	// normalMatrix = ((View * Model)⁻¹)ᵀ = (Model⁻¹)ᵀ * (View⁻¹)ᵀ = ... = R * [S⁻¹] * (View⁻¹)ᵀ but:
	// ViewMatrix with no translation: has only rotation features which is again orthogonal
	//		so  View⁻¹ = Viewᵀ --> (View⁻¹)ᵀ = (Viewᵀ)ᵀ = View
	// therefore: normalMatrix = R * [S⁻¹] * ViewNoTrans

	if (this->transformationApplied == false)
	{
		return viewNoTranslation;
	}

	return viewNoTranslation * this->getNormalMatrix();
}

MeshlayoutDescription VulkanObject::getVboLayout() const
{
	assert(this->model != nullptr && "Model not set");
	return this->model->getVboLayout();
};

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