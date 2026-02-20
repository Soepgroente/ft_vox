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
	vec3	ambientClr;
	vec3	diffuseClr;
	vec3	specularClr;
	float	shininess;
	float	opacity;
	float	illuminationModel;
	float	refractionIndex;
	bool	smoothShading;
};

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
	vec3	translation{};
	vec3	scale{1.0f, 1.0f, 1.0f};
	vec3	rotation{};

	mat4	matrix4() const noexcept;
	mat4	matrix4(const vec3& rotationCenter) const noexcept;
	mat3	normalMatrix() const noexcept;
};

class VulkanObject
{
	public:

	using id_t = uint32_t;
	using Map = std::unordered_map<id_t, VulkanObject>;

	VulkanObject() = delete;
	~VulkanObject();
	VulkanObject(const VulkanObject& other) = delete;
	VulkanObject& operator=(const VulkanObject& other) = delete;
	VulkanObject(VulkanObject&& other) = default;
	VulkanObject& operator=(VulkanObject&& other) = default;

	static VulkanObject	createVulkanObject() { return VulkanObject(++currentID); }
	static id_t		currentID;

	std::shared_ptr<VulkanModel>	model;
	vec3							color;
	TransformComponent				transform{};
	
	id_t	getID() const noexcept { return id; }

	std::vector<vec3> positions{ 0.0f, 0.0f, 0.0f };
	
	private:

	VulkanObject(id_t objID);

	id_t		id;
};

std::ostream&	operator<<(std::ostream& os, const ObjInfo& obj);
ImageInfo		loadImage(const std::string& imagePath);
std::vector<ObjInfo>	parseOBJFile(const std::string& objFilePath);

} // namespace ve