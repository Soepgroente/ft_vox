#pragma once

#include "Vulkan.hpp"
#include "Vectors.hpp"

#include <cstdint>
#include <unordered_map>
#include <array>
#include <deque>


namespace vox {

inline constexpr uint32_t	VERTEX_PER_VOXEL = 24U;	// number of vertexes per voxel
inline constexpr uint32_t	INDEX_PER_VOXEL = 36U;	// number of vertex indexes per voxel

using VertexArray = std::array<ve::VulkanModel::Vertex,VERTEX_PER_VOXEL>;
using IndexArray = std::array<uint32_t, INDEX_PER_VOXEL>;

inline VertexArray VOXEL_VERTEXES{
    // face FRONT (z = +0.5)
    ve::VulkanModel::Vertex{vec3{ -0.5f, -0.5f,  0.5f }, ve::generateRandomColor(), vec3(0.0f), vec2{ 0.0f, 1.0f }},
    ve::VulkanModel::Vertex{vec3{  0.5f, -0.5f,  0.5f }, ve::generateRandomColor(), vec3(0.0f), vec2{ 1.0f, 1.0f }},
    ve::VulkanModel::Vertex{vec3{  0.5f,  0.5f,  0.5f }, ve::generateRandomColor(), vec3(0.0f), vec2{ 1.0f, 0.0f }},
    ve::VulkanModel::Vertex{vec3{ -0.5f,  0.5f,  0.5f }, ve::generateRandomColor(), vec3(0.0f), vec2{ 0.0f, 0.0f }},
    //face BACK (z = -0.5)
    ve::VulkanModel::Vertex{vec3{  0.5f, -0.5f, -0.5f }, ve::generateRandomColor(), vec3(0.0f), vec2{ 0.0f, 1.0f }},
    ve::VulkanModel::Vertex{vec3{ -0.5f, -0.5f, -0.5f }, ve::generateRandomColor(), vec3(0.0f), vec2{ 1.0f, 1.0f }},
    ve::VulkanModel::Vertex{vec3{ -0.5f,  0.5f, -0.5f }, ve::generateRandomColor(), vec3(0.0f), vec2{ 1.0f, 0.0f }},
    ve::VulkanModel::Vertex{vec3{  0.5f,  0.5f, -0.5f }, ve::generateRandomColor(), vec3(0.0f), vec2{ 0.0f, 0.0f }},
    // face LEFT (x = -0.5)
    ve::VulkanModel::Vertex{vec3{ -0.5f, -0.5f, -0.5f }, ve::generateRandomColor(), vec3(0.0f), vec2{ 0.0f, 1.0f }},
    ve::VulkanModel::Vertex{vec3{ -0.5f, -0.5f,  0.5f }, ve::generateRandomColor(), vec3(0.0f), vec2{ 1.0f, 1.0f }},
    ve::VulkanModel::Vertex{vec3{ -0.5f,  0.5f,  0.5f }, ve::generateRandomColor(), vec3(0.0f), vec2{ 1.0f, 0.0f }},
    ve::VulkanModel::Vertex{vec3{ -0.5f,  0.5f, -0.5f }, ve::generateRandomColor(), vec3(0.0f), vec2{ 0.0f, 0.0f }},
    // face RIGHT (x = +0.5)
    ve::VulkanModel::Vertex{vec3{  0.5f, -0.5f,  0.5f }, ve::generateRandomColor(), vec3(0.0f), vec2{ 0.0f, 1.0f }},
    ve::VulkanModel::Vertex{vec3{  0.5f, -0.5f, -0.5f }, ve::generateRandomColor(), vec3(0.0f), vec2{ 1.0f, 1.0f }},
    ve::VulkanModel::Vertex{vec3{  0.5f,  0.5f, -0.5f }, ve::generateRandomColor(), vec3(0.0f), vec2{ 1.0f, 0.0f }},
    ve::VulkanModel::Vertex{vec3{  0.5f,  0.5f,  0.5f }, ve::generateRandomColor(), vec3(0.0f), vec2{ 0.0f, 0.0f }},
    // face TOP (y = +0.5)
    ve::VulkanModel::Vertex{vec3{ -0.5f,  0.5f,  0.5f }, ve::generateRandomColor(), vec3(0.0f), vec2{ 0.0f, 1.0f }},
    ve::VulkanModel::Vertex{vec3{  0.5f,  0.5f,  0.5f }, ve::generateRandomColor(), vec3(0.0f), vec2{ 1.0f, 1.0f }},
    ve::VulkanModel::Vertex{vec3{  0.5f,  0.5f, -0.5f }, ve::generateRandomColor(), vec3(0.0f), vec2{ 1.0f, 0.0f }},
    ve::VulkanModel::Vertex{vec3{ -0.5f,  0.5f, -0.5f }, ve::generateRandomColor(), vec3(0.0f), vec2{ 0.0f, 0.0f }},
    // face BOTTOM (y = -0.5)
    ve::VulkanModel::Vertex{vec3{ -0.5f, -0.5f, -0.5f }, ve::generateRandomColor(), vec3(0.0f), vec2{ 0.0f, 1.0f }},
    ve::VulkanModel::Vertex{vec3{  0.5f, -0.5f, -0.5f }, ve::generateRandomColor(), vec3(0.0f), vec2{ 1.0f, 1.0f }},
    ve::VulkanModel::Vertex{vec3{  0.5f, -0.5f,  0.5f }, ve::generateRandomColor(), vec3(0.0f), vec2{ 1.0f, 0.0f }},
    ve::VulkanModel::Vertex{vec3{ -0.5f, -0.5f,  0.5f }, ve::generateRandomColor(), vec3(0.0f), vec2{ 0.0f, 0.0f }}
};

inline constexpr IndexArray VOXEL_VERTEX_INDEXES{
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

VertexArray	getVertexRelativeMonoTexture( vec3 const& = vec3(0.0f) );
VertexArray	getVertexRelativeAtlasTexture( vec3 const& = vec3(0.0f) );
IndexArray	getIndexRelative( uint32_t = 0U );


class WorldGenerator {
	public:
		class HistoryWorlds {
			// a world with position (x, y) becomes (x * sizeX, 0, y * sizeZ) in local space 
			public:
				explicit HistoryWorlds( uint32_t max ) : max(max) {};
				~HistoryWorlds( void ) noexcept = default;
				HistoryWorlds( HistoryWorlds const& ) = delete;
				HistoryWorlds( HistoryWorlds&& ) = delete;
				HistoryWorlds& operator=( HistoryWorlds const& ) = delete;
				HistoryWorlds& operator=( HistoryWorlds&& ) = delete;

				void add(vec2i const& newPos);
				bool hasVisited(vec2i const& pos) const;

			private:
				uint32_t							max;		// max number of positions stored
				std::deque<vec2i>					history;	// using FIFO for storing position history
				std::unordered_map<vec2i,uint32_t>	counter;	// using unord. map for fast lookup 
		};

		explicit WorldGenerator( vec3ui const& worldSize, uint32_t maxWorldsStored ) : 
			history(maxWorldsStored), 
			worldSize(worldSize) {};
		~WorldGenerator( void ) = default;
		WorldGenerator( WorldGenerator const& ) = delete;
		WorldGenerator( WorldGenerator&& ) = delete;
		WorldGenerator& operator=( WorldGenerator const& ) = delete;
		WorldGenerator& operator=( WorldGenerator&& ) = delete;

		void	init( vec3 const& );
		bool	spawnCloseByWorlds( vec3 const& );
		bool	addeNewWorld( vec2i const& );

		ve::VulkanModel::Builder const&	getBuilder( void ) const noexcept { return builder; };
		ve::VulkanModel::Builder&		getBuilder( void ) noexcept { return builder; };

	private:
		void	fillBufferPlainTerrain( vec2i const& );
		// worlds are stored with 2D XY position,
		// local positions are stored in XYZ (its Z is the Y of the world pos)
		vec2i	worldPosFromLocalPos( vec3 const& ) const noexcept;
		vec3	localPosFromWorldPos( vec2i const& ) const noexcept;

		HistoryWorlds				history;
		vec3ui						worldSize;
		ve::VulkanModel::Builder	builder;
};

float	perlinNoise(float x, float y, ui32 seed);
float	randomNoise(float, float, ui32& seed);

}	// namespace vox
