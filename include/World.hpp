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

VertexVector	getVertexRelativeMonoTexture( vec3 const& = vec3(0.0f) );
VertexVector	getVertexRelativeAtlasTexture( vec3 const& = vec3(0.0f) );
IndexVector		getIndexRelative( uint32_t = 0U );

class VoxelMap;

class World {
	public:
		explicit World( vec3i const&, vec3ui const& );
		World( void ) = default;
		~World( void ) noexcept = default;
		World( World const& ) = delete;
		World( World&& ) = default;
		World& operator=( World const& ) = delete;
		World& operator=( World&& ) = default;

		VertexVector const&	getVertexes( void ) const noexcept { return this->vertexes; };
		VertexVector&		getVertexes( void ) noexcept { return this->vertexes; };
		IndexVector			getIndexes( u_int32_t start ) const noexcept { return getIndexRelative(start); };
		uint32_t			getVertexSize( void ) const noexcept { return this->vertexes.size(); }
		float				getWeight( vec3i const& ) const noexcept;
		void				updateLastAccess( void ) noexcept;

		static constexpr float ALPHA = 0.8f;	// weight for distance
		static constexpr float BETA = 0.2f;		// weight for the time
		static inline VoxelMap*	voxelMap;

	private:
		vec3i			worldPos;
		vec3ui			worldSize;
		VertexVector	vertexes;
		Time			lastAccess;

};


class WorldNavigator {
	public:
		explicit WorldNavigator() : 
			worldSize(Config::worldSize, Config::worldHeight, Config::worldSize),
			totVoxels(0U),
			currentWorldPos(0U) {};
		~WorldNavigator( void ) = default;
		WorldNavigator( WorldNavigator const& ) = delete;
		WorldNavigator( WorldNavigator&& ) = delete;
		WorldNavigator& operator=( WorldNavigator const& ) = delete;
		WorldNavigator& operator=( WorldNavigator&& ) = delete;

		bool	spawnCloseByWorlds( vec3 const& );
		bool 	spawnCloseByWorlds(vec3 const& start, ThreadManager& threads);
		size_t	getMemoryUsed( void ) const noexcept;
		bool	borderCrossed( vec3 const& ) const noexcept;

		std::unique_ptr<ve::VulkanModel> createNewModel( ve::VulkanDevice& ) const;
		bool	addNewWorld( vec3i const& );

	private:
		vec3i	findFurthestWorld( void ) const noexcept;
		vec3i	worldPosFromPlayerPos( vec3 const& ) const noexcept;

		vec3ui							worldSize;			// 3D dimension of every world
		uint32_t						totVoxels;			// total voxel generated in every world
		// map of the existing chunks/worlds (note: positions are stored using integers, that 
		// represent the 3D indexes of the world, not their actual distance frm the origin)
		std::unordered_map<vec3i,World>	worlds;
		vec3i							currentWorldPos;	// last position known of the player
};

float	perlin(float x, float y, float z);
float	randomNoise(float, float, ui32& seed);

}	// namespace vox
