#pragma once

#include "Vulkan.hpp"
#include "Vectors.hpp"

#include <cstdint>
#include <unordered_map>
#include <array>


namespace vox {

inline constexpr uint32_t	VERTEX_PER_VOXEL = 24U;	// number of vertexes per voxel
inline constexpr uint32_t	INDEX_PER_VOXEL = 36U;	// number of vertex indexes per voxel

// 1 voxel corresponds to:
// vertexes = 24 (vertexes per voxel) * 44 (bytes per vertex) = 1056 b = 1.031 KiB
// indexes = 36 (indexes per voxel) * 4 (bytes per index [uint]) = 144 b
// total = 1200 b = 1.171 KiB
//
// 32 * 32 voxels correspond to:
// vertexes = 32 * 32 * 1056 b = 1081344 b = 1056 KiB = 1.031 MiB
// indexes = 32 * 32 * 144 b = 147456 b = 144 KiB
// total = 1228800 b = 1200 KiB = 1.171 MiB
//
// to have a limit around 150 MiB, 2*17 = 128 voxels are required
// vertexes = 128 * 32 * 32 * 1056 b = 138412032 b = 135168 KiB = 132 MiB
// indexes = 128 * 32 * 32 * 144 b = 18874368 b = 18432 KiB = 18 MiB
// total = 157286400 b = 153600 KiB = 150 MiB
inline constexpr uint32_t	MAX_WORLDS = 128U;

inline constexpr std::array<ve::VulkanModel::Vertex,VERTEX_PER_VOXEL> VOXEL_VERTEXES{
    // face FRONT (z = +0.5)
    ve::VulkanModel::Vertex{vec3{ -0.5f, -0.5f,  0.5f }, vec3(0.0f), vec3(0.0f), vec2{ 0.0f, 1.0f }},
    ve::VulkanModel::Vertex{vec3{  0.5f, -0.5f,  0.5f }, vec3(0.0f), vec3(0.0f), vec2{ 1.0f, 1.0f }},
    ve::VulkanModel::Vertex{vec3{  0.5f,  0.5f,  0.5f }, vec3(0.0f), vec3(0.0f), vec2{ 1.0f, 0.0f }},
    ve::VulkanModel::Vertex{vec3{ -0.5f,  0.5f,  0.5f }, vec3(0.0f), vec3(0.0f), vec2{ 0.0f, 0.0f }},
    //face BACK (z = -0.5)
    ve::VulkanModel::Vertex{vec3{  0.5f, -0.5f, -0.5f }, vec3(0.0f), vec3(0.0f), vec2{ 0.0f, 1.0f }},
    ve::VulkanModel::Vertex{vec3{ -0.5f, -0.5f, -0.5f }, vec3(0.0f), vec3(0.0f), vec2{ 1.0f, 1.0f }},
    ve::VulkanModel::Vertex{vec3{ -0.5f,  0.5f, -0.5f }, vec3(0.0f), vec3(0.0f), vec2{ 1.0f, 0.0f }},
    ve::VulkanModel::Vertex{vec3{  0.5f,  0.5f, -0.5f }, vec3(0.0f), vec3(0.0f), vec2{ 0.0f, 0.0f }},
    // face LEFT (x = -0.5)
    ve::VulkanModel::Vertex{vec3{ -0.5f, -0.5f, -0.5f }, vec3(0.0f), vec3(0.0f), vec2{ 0.0f, 1.0f }},
    ve::VulkanModel::Vertex{vec3{ -0.5f, -0.5f,  0.5f }, vec3(0.0f), vec3(0.0f), vec2{ 1.0f, 1.0f }},
    ve::VulkanModel::Vertex{vec3{ -0.5f,  0.5f,  0.5f }, vec3(0.0f), vec3(0.0f), vec2{ 1.0f, 0.0f }},
    ve::VulkanModel::Vertex{vec3{ -0.5f,  0.5f, -0.5f }, vec3(0.0f), vec3(0.0f), vec2{ 0.0f, 0.0f }},
    // face RIGHT (x = +0.5)
    ve::VulkanModel::Vertex{vec3{  0.5f, -0.5f,  0.5f }, vec3(0.0f), vec3(0.0f), vec2{ 0.0f, 1.0f }},
    ve::VulkanModel::Vertex{vec3{  0.5f, -0.5f, -0.5f }, vec3(0.0f), vec3(0.0f), vec2{ 1.0f, 1.0f }},
    ve::VulkanModel::Vertex{vec3{  0.5f,  0.5f, -0.5f }, vec3(0.0f), vec3(0.0f), vec2{ 1.0f, 0.0f }},
    ve::VulkanModel::Vertex{vec3{  0.5f,  0.5f,  0.5f }, vec3(0.0f), vec3(0.0f), vec2{ 0.0f, 0.0f }},
    // face TOP (y = +0.5)
    ve::VulkanModel::Vertex{vec3{ -0.5f,  0.5f,  0.5f }, vec3(0.0f), vec3(0.0f), vec2{ 0.0f, 1.0f }},
    ve::VulkanModel::Vertex{vec3{  0.5f,  0.5f,  0.5f }, vec3(0.0f), vec3(0.0f), vec2{ 1.0f, 1.0f }},
    ve::VulkanModel::Vertex{vec3{  0.5f,  0.5f, -0.5f }, vec3(0.0f), vec3(0.0f), vec2{ 1.0f, 0.0f }},
    ve::VulkanModel::Vertex{vec3{ -0.5f,  0.5f, -0.5f }, vec3(0.0f), vec3(0.0f), vec2{ 0.0f, 0.0f }},
    // face BOTTOM (y = -0.5)
    ve::VulkanModel::Vertex{vec3{ -0.5f, -0.5f, -0.5f }, vec3(0.0f), vec3(0.0f), vec2{ 0.0f, 1.0f }},
    ve::VulkanModel::Vertex{vec3{  0.5f, -0.5f, -0.5f }, vec3(0.0f), vec3(0.0f), vec2{ 1.0f, 1.0f }},
    ve::VulkanModel::Vertex{vec3{  0.5f, -0.5f,  0.5f }, vec3(0.0f), vec3(0.0f), vec2{ 1.0f, 0.0f }},
    ve::VulkanModel::Vertex{vec3{ -0.5f, -0.5f,  0.5f }, vec3(0.0f), vec3(0.0f), vec2{ 0.0f, 0.0f }}
};

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

VertexVector	getVertexRelativeMonoTexture( vec3 const& = vec3(0.0f) );
VertexVector	getVertexRelativeAtlasTexture( vec3 const& = vec3(0.0f) );
IndexVector		getIndexRelative( uint32_t = 0U );


class World {
	public:
		explicit World( vec3i const&, vec3ui const& );
		World( void ) = default;
		~World( void ) noexcept = default;
		World( World const& ) = delete;
		World( World&& ) = default;
		World& operator=( World const& ) = delete;
		World& operator=( World&& ) = default;

		VertexVector const&	getVertexes( void ) const { return this->vertexes; };
		VertexVector&		getVertexes( void ) { return this->vertexes; };
		IndexVector			getIndexes( u_int32_t start ) const { return getIndexRelative(start); };
		uint32_t			getVertexSize( void ) const noexcept { return this->vertexes.size(); }

	private:
		vec3i			worldPos;
		vec3ui			worldSize;
		VertexVector	vertexes;
};


class WorldGenerator {
	public:
		explicit WorldGenerator( uint32_t worldSize ) : 
			worldSize(worldSize),
			totVoxels(0U),
			currentWorldPos(0U) {};
		~WorldGenerator( void ) = default;
		WorldGenerator( WorldGenerator const& ) = delete;
		WorldGenerator( WorldGenerator&& ) = delete;
		WorldGenerator& operator=( WorldGenerator const& ) = delete;
		WorldGenerator& operator=( WorldGenerator&& ) = delete;

		void	init( vec3 const& );
		bool	spawnCloseByWorlds( vec3 const& );
		size_t	getMemoryUsed( void ) noexcept;

		std::unique_ptr<ve::VulkanModel>	createNewModel( ve::VulkanDevice& );

	private:
		bool	addeNewWorld( vec3i const& );
		vec3i	findFurthestWorld( void ) noexcept;
		vec3i	worldPosFromLocalPos( vec3 const& ) const noexcept;

		vec3ui							worldSize;	// 3D dimension of every world
		uint32_t						totVoxels;	// total voxel generated in every world
		std::unordered_map<vec3i,World>	worlds;		// using unord. map for fast lookup 
		vec3i							currentWorldPos;
};

}	// namespace vox
