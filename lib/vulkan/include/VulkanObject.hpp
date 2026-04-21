#pragma once

#include "Vectors.hpp"
#include "VulkanModel.hpp"

#include <memory>
#include <map>
#include <string>
#include <unordered_map>

namespace ve {

struct ImageInfo
{
	const unsigned char*	imageData;
	int		width;
	int		height;
	int		channels;
};

struct Material
{
	std::string	name;
	vec4	ambientClr;
	vec4	diffuseClr;
	vec4	specularClr;
	int32_t	shininess;
	float	opacity;
	int32_t	refractionIndex;
	int32_t	illuminationModel;
	bool	smoothShading;
};

struct MeshMaterial
{
	vec4	ambientClr;			// range [0-1] - indirect light color (darker than diffuse)
	vec4	diffuseClr;			// range [0-1] - color of the mesh
	vec4	specularClr;		// range [0-1] - reflex of the light
	float	shininess;			// range [1.0-256.0] - low (2-8): opaque, high (64-256) shiny/metal
	float	opacity;			// alpha of diffuse
	int32_t	refractionIndex;	// range [1-2.42...] - index of refraction, 1.0: air, 1.33 h2o, 1.5 glass
	int32_t	illuminationModel;	// range [0-10] - 0: no lighting only texture, 1: ambient + diffuse, 2: ambient + diffuse + specular
};

static_assert(sizeof(MeshMaterial) == 64);

struct ObjComponent
{
	std::vector<std::vector<uint32_t>>	faceIndices;
	std::vector<std::vector<uint32_t>>	textureIndices;
	std::vector<std::vector<uint32_t>>	normalIndices;
	std::string							matName;
};

struct ObjInfo
{
	std::string					name;
	std::string					mtlFile;
	std::vector<vec3>			vertices;
	std::vector<vec2>			textureCoords;
	std::vector<vec3>			normals;
	std::vector<vec3>			colors;
	std::vector<ObjComponent>	components;
	std::map<std::string, Material>		materials;
};

struct TransformComponent
{
	vec3	translation{0.0f};
	vec3	scale{1.0f, 1.0f, 1.0f};
	quat	rotation{};

	mat4	matrix4() const noexcept;
	mat4	matrix4(const vec3& rotationCenter) const noexcept;
	mat3	normalMatrix() const noexcept;
};

class VulkanObject
{
	public:
		VulkanObject() : id(currentID++) {};
		VulkanObject(const VulkanObject& other) = delete;
		VulkanObject(VulkanObject&& other) = default;
		VulkanObject& operator=(const VulkanObject& other) = delete;
		VulkanObject& operator=(VulkanObject&& other) = delete;
		~VulkanObject() = default;

		void	rotate( vec3 const& axis, float angle ) noexcept;
		void	translate( vec3 const& translation ) noexcept;
		void	scale( vec3 const& scale ) noexcept;
		void	scale( float scale ) noexcept;
		
		void	bindBuffer(VkCommandBuffer commandBuffer) const;
		void	draw(VkCommandBuffer commandBuffer) const;
		
		void							setModel(std::shared_ptr<VulkanModel> newModel) noexcept { this->model = newModel; };
		std::shared_ptr<VulkanModel>	getModel() const;
		void							setMaterial(MeshMaterial const& material) noexcept { this->materialData = material; };
		MeshMaterial const&				getMaterial() const noexcept { return this->materialData; };
		mat4							getModelMatrix() const noexcept;
		mat4							getNormalMatrix() const noexcept;
		mat4							getNormalViewMatrix(const mat4& viewNoTranslation) const noexcept;
		uint32_t						getID() const noexcept { return this->id; }
		MeshlayoutDescription			getVboLayout() const;

	private:
		uint32_t						id;
		std::shared_ptr<VulkanModel>	model{nullptr};
		TransformComponent				transform{};
		MeshMaterial					materialData{};
		bool							transformationApplied{false};
		bool							uniformScale{true};

		static uint32_t		currentID;
};

std::ostream&	operator<<(std::ostream& os, const ObjInfo& obj);
ImageInfo		loadImage(const std::string& imagePath);
std::vector<ObjInfo>	parseOBJFile(const std::string& objFilePath);

} // namespace ve