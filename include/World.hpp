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

// Hard-coded VBO (vertex+normal+textureUV data) of a voxel
inline constexpr std::array<ve::VulkanModel::Vertex,VERTEX_PER_VOXEL> VOXEL_VERTEXES{
	// face FRONT (z = +0.5)
	ve::VulkanModel::Vertex{vec3{ -0.5f, -0.5f,  0.5f }, vec3(0.0f), vec2{ 0.0f, 1.0f }},
	ve::VulkanModel::Vertex{vec3{  0.5f, -0.5f,  0.5f }, vec3(0.0f), vec2{ 1.0f, 1.0f }},
	ve::VulkanModel::Vertex{vec3{  0.5f,  0.5f,  0.5f }, vec3(0.0f), vec2{ 1.0f, 0.0f }},
	ve::VulkanModel::Vertex{vec3{ -0.5f,  0.5f,  0.5f }, vec3(0.0f), vec2{ 0.0f, 0.0f }},
	//face BACK (z = -0.5)
	ve::VulkanModel::Vertex{vec3{  0.5f, -0.5f, -0.5f }, vec3(0.0f), vec2{ 0.0f, 1.0f }},
	ve::VulkanModel::Vertex{vec3{ -0.5f, -0.5f, -0.5f }, vec3(0.0f), vec2{ 1.0f, 1.0f }},
	ve::VulkanModel::Vertex{vec3{ -0.5f,  0.5f, -0.5f }, vec3(0.0f), vec2{ 1.0f, 0.0f }},
	ve::VulkanModel::Vertex{vec3{  0.5f,  0.5f, -0.5f }, vec3(0.0f), vec2{ 0.0f, 0.0f }},
	// face LEFT (x = -0.5)
	ve::VulkanModel::Vertex{vec3{ -0.5f, -0.5f, -0.5f }, vec3(0.0f), vec2{ 0.0f, 1.0f }},
	ve::VulkanModel::Vertex{vec3{ -0.5f, -0.5f,  0.5f }, vec3(0.0f), vec2{ 1.0f, 1.0f }},
	ve::VulkanModel::Vertex{vec3{ -0.5f,  0.5f,  0.5f }, vec3(0.0f), vec2{ 1.0f, 0.0f }},
	ve::VulkanModel::Vertex{vec3{ -0.5f,  0.5f, -0.5f }, vec3(0.0f), vec2{ 0.0f, 0.0f }},
	// face RIGHT (x = +0.5)
	ve::VulkanModel::Vertex{vec3{  0.5f, -0.5f,  0.5f }, vec3(0.0f), vec2{ 0.0f, 1.0f }},
	ve::VulkanModel::Vertex{vec3{  0.5f, -0.5f, -0.5f }, vec3(0.0f), vec2{ 1.0f, 1.0f }},
	ve::VulkanModel::Vertex{vec3{  0.5f,  0.5f, -0.5f }, vec3(0.0f), vec2{ 1.0f, 0.0f }},
	ve::VulkanModel::Vertex{vec3{  0.5f,  0.5f,  0.5f }, vec3(0.0f), vec2{ 0.0f, 0.0f }},
	// face TOP (y = +0.5)
	ve::VulkanModel::Vertex{vec3{ -0.5f,  0.5f,  0.5f }, vec3(0.0f), vec2{ 0.0f, 1.0f }},
	ve::VulkanModel::Vertex{vec3{  0.5f,  0.5f,  0.5f }, vec3(0.0f), vec2{ 1.0f, 1.0f }},
	ve::VulkanModel::Vertex{vec3{  0.5f,  0.5f, -0.5f }, vec3(0.0f), vec2{ 1.0f, 0.0f }},
	ve::VulkanModel::Vertex{vec3{ -0.5f,  0.5f, -0.5f }, vec3(0.0f), vec2{ 0.0f, 0.0f }},
	// face BOTTOM (y = -0.5)
	ve::VulkanModel::Vertex{vec3{ -0.5f, -0.5f, -0.5f }, vec3(0.0f), vec2{ 0.0f, 1.0f }},
	ve::VulkanModel::Vertex{vec3{  0.5f, -0.5f, -0.5f }, vec3(0.0f), vec2{ 1.0f, 1.0f }},
	ve::VulkanModel::Vertex{vec3{  0.5f, -0.5f,  0.5f }, vec3(0.0f), vec2{ 1.0f, 0.0f }},
	ve::VulkanModel::Vertex{vec3{ -0.5f, -0.5f,  0.5f }, vec3(0.0f), vec2{ 0.0f, 0.0f }}
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

VertexVector		getVertexRelativeMonoTexture( vec3 const& = vec3(0.0f) );
VertexVector		getVertexRelativeAtlasTexture( vec3 const& = vec3(0.0f) );
std::vector<vec3>	getVertexRelative( vec3 const& = vec3(0.0f) );
IndexVector			getIndexRelative( uint32_t = 0U );

float	perlin(float x, float y, float z);
float	randomNoise(float, float, ui32& seed);

}	// namespace vox
