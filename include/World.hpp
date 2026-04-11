#pragma once

#include "ThreadManager.hpp"
#include "Vulkan.hpp"
#include "Vectors.hpp"
#include "Stopwatch.hpp"
#include "VoxelMap.hpp"
#include "Config.hpp"

#include <cstdint>
#include <unordered_map>
#include <array>


namespace vox {

inline constexpr uint32_t	VERTEX_PER_VOXEL = 24U;	// number of vertexes per voxel
inline constexpr uint32_t	INDEX_PER_VOXEL = 36U;	// number of vertex indexes per voxel
inline constexpr float		VOXEL_SIZE = 1.0f;		// length of a voxel edge

/*
* 1 voxel corresponds to:
* vertexes = 24 (vertexes per voxel) * 32 (bytes per vertex) = 768 b
* indexes = 36 (indexes per voxel) * 4 (bytes per index [uint]) = 144 b
* total = 912 b
*
* 32 * 32 voxels correspond to:
* vertexes = 32 * 32 * 768 b = 786432 b = 768 KiB
* indexes = 32 * 32 * 144 b = 147456 b = 144 KiB
* total = 933888 b = 912 KiB
*
* an arbitrary limit of 114 MiB, means 128 (worlds) * 32 * 32 (voxels per chunk) = 2^17 voxels
* vertexes = 128 * 32 * 32 * 768 b = 100663296 b = 98304 KiB = 96 MiB
* indexes = 128 * 32 * 32 * 144 b = 18874368 b = 18432 KiB = 18 MiB
* total = 119537664 b = 116736 KiB = 114 MiB
* 
* @note this will change since the size of a world will not always be 32 * 32 voxels
*/
inline constexpr uint32_t	MAX_WORLDS = 128U;

enum class VertexFaces : size_t
{
	FRONT = 0,
	BACK = 4,
	LEFT = 8,
	RIGHT = 12,
	TOP = 16,
	BOTTOM = 20
};

static constexpr float W = 1.0f / 4.0f;  // width of a tile
static constexpr float H = 1.0f / 3.0f;  // height of a tile
static constexpr float padding = 0.004f;

// Hard-coded VBO (vertex+normal+textureUV data) of a voxel
inline constexpr std::array<ve::VulkanModel::Vertex,VERTEX_PER_VOXEL> VOXEL_VERTEXES{
	// face FRONT (z = +0.5)
	ve::VulkanModel::Vertex{vec3{ -0.5f, -0.5f,  0.5f }, vec3::forward(), vec2{ W + padding, 3 * H - padding }},
	ve::VulkanModel::Vertex{vec3{  0.5f, -0.5f,  0.5f }, vec3::forward(), vec2{ 2 * W - padding, 3 * H - padding }},
	ve::VulkanModel::Vertex{vec3{  0.5f,  0.5f,  0.5f }, vec3::forward(), vec2{ 2 * W - padding, 2 * H + padding }},
	ve::VulkanModel::Vertex{vec3{ -0.5f,  0.5f,  0.5f }, vec3::forward(), vec2{ W + padding, 2 * H + padding }},
	//face BACK (z = -0.5)
	ve::VulkanModel::Vertex{vec3{  0.5f, -0.5f, -0.5f }, vec3::backward(), vec2{ 2 * W - padding, padding }},
	ve::VulkanModel::Vertex{vec3{ -0.5f, -0.5f, -0.5f }, vec3::backward(), vec2{ W + padding, padding }},
	ve::VulkanModel::Vertex{vec3{ -0.5f,  0.5f, -0.5f }, vec3::backward(), vec2{ W + padding, H - padding }},
	ve::VulkanModel::Vertex{vec3{  0.5f,  0.5f, -0.5f }, vec3::backward(), vec2{ 2 * W - padding, H - padding }},
	// face LEFT (x = -0.5)
	ve::VulkanModel::Vertex{vec3{ -0.5f, -0.5f, -0.5f }, vec3::left(), vec2{ padding, H + padding }},
	ve::VulkanModel::Vertex{vec3{ -0.5f, -0.5f,  0.5f }, vec3::left(), vec2{ padding, 2 * H - padding }},
	ve::VulkanModel::Vertex{vec3{ -0.5f,  0.5f,  0.5f }, vec3::left(), vec2{ W - padding, 2 * H - padding }},
	ve::VulkanModel::Vertex{vec3{ -0.5f,  0.5f, -0.5f }, vec3::left(), vec2{ W - padding, H + padding }},
	// face RIGHT (x = +0.5)
	ve::VulkanModel::Vertex{vec3{  0.5f, -0.5f,  0.5f }, vec3::right(), vec2{ 3 * W - padding, 2 * H - padding }},
	ve::VulkanModel::Vertex{vec3{  0.5f, -0.5f, -0.5f }, vec3::right(), vec2{ 3 * W - padding, H + padding }},
	ve::VulkanModel::Vertex{vec3{  0.5f,  0.5f, -0.5f }, vec3::right(), vec2{ 2 * W + padding, H + padding }},
	ve::VulkanModel::Vertex{vec3{  0.5f,  0.5f,  0.5f }, vec3::right(), vec2{ 2 * W + padding, 2 * H - padding }},
	// face TOP (y = +0.5)
	ve::VulkanModel::Vertex{vec3{ -0.5f,  0.5f,  0.5f }, vec3::up(), vec2{ W + padding, 2 * H - padding }},
	ve::VulkanModel::Vertex{vec3{  0.5f,  0.5f,  0.5f }, vec3::up(), vec2{ 2 * W - padding, 2 * H - padding }},
	ve::VulkanModel::Vertex{vec3{  0.5f,  0.5f, -0.5f }, vec3::up(), vec2{ 2 * W - padding, H + padding }},
	ve::VulkanModel::Vertex{vec3{ -0.5f,  0.5f, -0.5f }, vec3::up(), vec2{ W + padding, H + padding }},
	// face BOTTOM (y = -0.5)
	ve::VulkanModel::Vertex{vec3{ -0.5f, -0.5f, -0.5f }, vec3::down(), vec2{ 3 * W + padding, H + padding }},
	ve::VulkanModel::Vertex{vec3{  0.5f, -0.5f, -0.5f }, vec3::down(), vec2{ 3 * W + padding, 2 * H - padding }},
	ve::VulkanModel::Vertex{vec3{  0.5f, -0.5f,  0.5f }, vec3::down(), vec2{ 4 * W - padding, 2 * H - padding }},
	ve::VulkanModel::Vertex{vec3{ -0.5f, -0.5f,  0.5f }, vec3::down(), vec2{ 4 * W - padding, H + padding }}
};

// hard-coded face indexes of a voxel
inline constexpr std::array<uint32_t, INDEX_PER_VOXEL> VOXEL_VERTEX_INDEXES{
	0U, 1U, 2U, 		// front face
	0U, 2U, 3U, 		// front face
	4U, 5U, 6U, 		// back face
	4U, 6U, 7U, 		// back face
	8U, 9U, 10U, 		// left face
	8U, 10U, 11U, 		// left face
	12U, 13U, 14U, 		// right face
	12U, 14U, 15U, 		// right face
	16U, 17U, 18U, 		// top face
	16U, 18U, 19U, 		// top face
	20U, 21U, 22U, 		// bottom face
	20U, 22U, 23U		// bottom face
};

using VertexVector = std::vector<ve::VulkanModel::Vertex>;
using IndexVector = std::vector<uint32_t>;

void	addVoxelFace(const vec3& voxelLocation, VertexVector& chunk, size_t faceIndex);
void	addVertexes(const vec3& position, VertexVector& chunk, int facesToAdd);
IndexVector		getIndexRelative( uint32_t = 0U );

VertexVector		getVertexRelativeMonoTexture( vec3 const& = vec3(0.0f) );
VertexVector		getVertexRelativeAtlasTexture( vec3 const& = vec3(0.0f) );
std::vector<vec3>	getVertexRelative( vec3 const& = vec3(0.0f) );

float	perlin(float x, float y, float z);
float	randomNoise(float, float, ui32& seed);

}	// namespace vox
