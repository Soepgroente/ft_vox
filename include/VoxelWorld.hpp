#pragma once

#include <array>
#include <cstdint>
#include <memory>
#include <map>
#include <iostream>
#include <unordered_map>
#include <deque>

#include "Config.hpp"
#include "Vulkan.hpp"


namespace vox {

inline constexpr uint32_t	VERTEX_PER_VOXEL = 24U;	// number of vertexes per voxel
inline constexpr uint32_t	VERTEX_PER_VOXEL_old = 8U;	// number of vertexes per voxel
inline constexpr uint32_t	INDEX_PER_VOXEL = 36U;	// number of vertex indexes per voxel

using VertexArray = std::array<ve::VulkanModel::Vertex,VERTEX_PER_VOXEL>;
using IndexArray = std::array<uint32_t, INDEX_PER_VOXEL>;

inline VertexArray VOXEL_VERTEXES{
    // face FRONT (z = +0.5)
    ve::VulkanModel::Vertex{vec3{ -0.5f, -0.5f,  0.5f }, vec3(0.0f), ve::generateRandomColor(), vec2{ 0.0f, 1.0f }},
    ve::VulkanModel::Vertex{vec3{  0.5f, -0.5f,  0.5f }, vec3(0.0f), ve::generateRandomColor(), vec2{ 1.0f, 1.0f }},
    ve::VulkanModel::Vertex{vec3{  0.5f,  0.5f,  0.5f }, vec3(0.0f), ve::generateRandomColor(), vec2{ 1.0f, 0.0f }},
    ve::VulkanModel::Vertex{vec3{ -0.5f,  0.5f,  0.5f }, vec3(0.0f), ve::generateRandomColor(), vec2{ 0.0f, 0.0f }},
    //face BACK (z = -0.5)
    ve::VulkanModel::Vertex{vec3{  0.5f, -0.5f, -0.5f }, vec3(0.0f), ve::generateRandomColor(), vec2{ 0.0f, 1.0f }},
    ve::VulkanModel::Vertex{vec3{ -0.5f, -0.5f, -0.5f }, vec3(0.0f), ve::generateRandomColor(), vec2{ 1.0f, 1.0f }},
    ve::VulkanModel::Vertex{vec3{ -0.5f,  0.5f, -0.5f }, vec3(0.0f), ve::generateRandomColor(), vec2{ 1.0f, 0.0f }},
    ve::VulkanModel::Vertex{vec3{  0.5f,  0.5f, -0.5f }, vec3(0.0f), ve::generateRandomColor(), vec2{ 0.0f, 0.0f }},
    // face LEFT (x = -0.5)
    ve::VulkanModel::Vertex{vec3{ -0.5f, -0.5f, -0.5f }, vec3(0.0f), ve::generateRandomColor(), vec2{ 0.0f, 1.0f }},
    ve::VulkanModel::Vertex{vec3{ -0.5f, -0.5f,  0.5f }, vec3(0.0f), ve::generateRandomColor(), vec2{ 1.0f, 1.0f }},
    ve::VulkanModel::Vertex{vec3{ -0.5f,  0.5f,  0.5f }, vec3(0.0f), ve::generateRandomColor(), vec2{ 1.0f, 0.0f }},
    ve::VulkanModel::Vertex{vec3{ -0.5f,  0.5f, -0.5f }, vec3(0.0f), ve::generateRandomColor(), vec2{ 0.0f, 0.0f }},
    // face RIGHT (x = +0.5)
    ve::VulkanModel::Vertex{vec3{  0.5f, -0.5f,  0.5f }, vec3(0.0f), ve::generateRandomColor(), vec2{ 0.0f, 1.0f }},
    ve::VulkanModel::Vertex{vec3{  0.5f, -0.5f, -0.5f }, vec3(0.0f), ve::generateRandomColor(), vec2{ 1.0f, 1.0f }},
    ve::VulkanModel::Vertex{vec3{  0.5f,  0.5f, -0.5f }, vec3(0.0f), ve::generateRandomColor(), vec2{ 1.0f, 0.0f }},
    ve::VulkanModel::Vertex{vec3{  0.5f,  0.5f,  0.5f }, vec3(0.0f), ve::generateRandomColor(), vec2{ 0.0f, 0.0f }},
    // face TOP (y = +0.5)
    ve::VulkanModel::Vertex{vec3{ -0.5f,  0.5f,  0.5f }, vec3(0.0f), ve::generateRandomColor(), vec2{ 0.0f, 1.0f }},
    ve::VulkanModel::Vertex{vec3{  0.5f,  0.5f,  0.5f }, vec3(0.0f), ve::generateRandomColor(), vec2{ 1.0f, 1.0f }},
    ve::VulkanModel::Vertex{vec3{  0.5f,  0.5f, -0.5f }, vec3(0.0f), ve::generateRandomColor(), vec2{ 1.0f, 0.0f }},
    ve::VulkanModel::Vertex{vec3{ -0.5f,  0.5f, -0.5f }, vec3(0.0f), ve::generateRandomColor(), vec2{ 0.0f, 0.0f }},
    // face BOTTOM (y = -0.5)
    ve::VulkanModel::Vertex{vec3{ -0.5f, -0.5f, -0.5f }, vec3(0.0f), ve::generateRandomColor(), vec2{ 0.0f, 1.0f }},
    ve::VulkanModel::Vertex{vec3{  0.5f, -0.5f, -0.5f }, vec3(0.0f), ve::generateRandomColor(), vec2{ 1.0f, 1.0f }},
    ve::VulkanModel::Vertex{vec3{  0.5f, -0.5f,  0.5f }, vec3(0.0f), ve::generateRandomColor(), vec2{ 1.0f, 0.0f }},
    ve::VulkanModel::Vertex{vec3{ -0.5f, -0.5f,  0.5f }, vec3(0.0f), ve::generateRandomColor(), vec2{ 0.0f, 0.0f }}
};

inline VertexArray VOXEL_VERTEXES_NO_TEXT{
    // face FRONT (z = +0.5)
	ve::VulkanModel::Vertex{vec3{ -0.5f, -0.5f,  0.5f }, vec3(0.0f), ve::generateRandomColor(), vec2(0.0f)},	// front-top-left corner
	ve::VulkanModel::Vertex{vec3{  0.5f, -0.5f,  0.5f }, vec3(0.0f), ve::generateRandomColor(), vec2(0.0f)},	// front-top-right corner
	ve::VulkanModel::Vertex{vec3{  0.5f,  0.5f,  0.5f }, vec3(0.0f), ve::generateRandomColor(), vec2(0.0f)},	// front-bottom-right corner
	ve::VulkanModel::Vertex{vec3{ -0.5f,  0.5f,  0.5f }, vec3(0.0f), ve::generateRandomColor(), vec2(0.0f)},	// front-bottom-left corner
    //face BACK (z = -0.5)
	ve::VulkanModel::Vertex{vec3{  0.5f, -0.5f, -0.5f }, vec3(0.0f), ve::generateRandomColor(), vec2(0.0f)},	// back-top-left corner
	ve::VulkanModel::Vertex{vec3{ -0.5f, -0.5f, -0.5f }, vec3(0.0f), ve::generateRandomColor(), vec2(0.0f)},	// back-top-right corner
	ve::VulkanModel::Vertex{vec3{ -0.5f,  0.5f, -0.5f }, vec3(0.0f), ve::generateRandomColor(), vec2(0.0f)},	// back-bottom-right corner
	ve::VulkanModel::Vertex{vec3{  0.5f,  0.5f, -0.5f }, vec3(0.0f), ve::generateRandomColor(), vec2(0.0f)}		// back-bottom-left corner
};

inline constexpr IndexArray VOXEL_VERTEX_INDEXES{
	0U, 1U, 2U, 		// front face
	0U, 2U, 3U, 		// front face
	4U, 5U, 6U, 		// back face
	4U, 6U, 7U, 		// back face
	8U, 9U, 10U, 		// bottom face
	8U, 10U, 11U, 		// bottom face
	12U, 13U, 14U, 		// top face
	12U, 14U, 15U, 		// top face
	16U, 17U, 18U, 		// right face
	16U, 18U, 19U, 		// right face
	20U, 21U, 22U, 		// left face
	20U, 22U, 23U		// left face
};

inline constexpr IndexArray VOXEL_VERTEX_INDEXES_NO_TEXT{
	0U, 1U, 2U,		// front face
	0U, 2U, 3U,		// front face
	4U, 7U, 6U,		// back face
	4U, 6U, 5U,		// back face
	0U, 1U, 5U,		// bottom face
	0U, 5U, 4U,		// bottom face
	3U, 7U, 6U,		// top face
	3U, 6U, 2U,		// top face
	1U, 2U, 6U,		// right face
	1U, 6U, 5U,		// right face
	0U, 3U, 7U,		// left face
	0U, 7U, 4U		// left face
};

VertexArray getVertexRelativeNoTextures( vec3 const& = vec3(0.0f), vec3ui const& = vec3ui(1U) );
VertexArray getVertexRelativeMonoTextures( vec3 const& = vec3(0.0f), vec3ui const& = vec3ui(1U) );
VertexArray getVertexRelativeAtlasTextures( vec3 const& = vec3(0.0f), vec3ui const& = vec3ui(1U) );
IndexArray getIndexRelative( uint32_t );


class VoxelWorld {
	public:
	    class WorldIterator {
			public:
				WorldIterator( vec3ui const& start, vec3ui const& limits ) : pos3D(start), limits(limits) {}
				~WorldIterator( void ) noexcept = default;
				WorldIterator( WorldIterator const& ) = delete;
				WorldIterator( WorldIterator&& ) = default;
				WorldIterator& operator=( WorldIterator const& ) = delete;
				WorldIterator& operator=( WorldIterator&& ) = delete;

				vec3ui const&	operator*( void ) const { return pos3D; }
				vec3ui&			operator*( void ) { return pos3D; }
				WorldIterator&	operator++( void );
				bool			operator!=( WorldIterator const& ) const;

			private:
				vec3ui			pos3D;
				vec3ui const&	limits;

			friend VoxelWorld;
		};

		~VoxelWorld( void ) = default;
		VoxelWorld( VoxelWorld const& ) = delete;
		VoxelWorld( VoxelWorld&& ) = default;
		VoxelWorld& operator=( VoxelWorld const& ) = delete;
		VoxelWorld& operator=( VoxelWorld&& ) = default;

		std::vector<bool>::reference		operator[]( vec3ui const& );
		std::vector<bool>::const_reference	operator[]( vec3ui const& ) const;

		bool	isVoxel( vec3ui const& ) const;
		void	setVoxel( vec3ui const&, bool );				// create/remove a single voxel in a 3D pos
		void	setVoxel( vec3ui const&, vec3ui const&, bool ); // create/remove a sector of voxels
		vec3ui	getBoxelSize( vec3ui const& ) const noexcept;	// greedy meshing

		WorldIterator begin( void ) const { return WorldIterator(vec3ui(0U), this->size);};
		WorldIterator begin( void ) { return WorldIterator(vec3ui(0U), this->size);};
		WorldIterator end( void ) const { return WorldIterator(this->size, this->size); };
		WorldIterator end( void ) { return WorldIterator(this->size, this->size); };

		static VoxelWorld voxelGenerator1( vec3ui const& );
		static VoxelWorld voxelGenerator2( vec3ui const& );
		static VoxelWorld voxelGenerator3( vec3ui const& );
		static VoxelWorld voxelGenerator4( vec3ui const& );
		static VoxelWorld voxelGenerator5( vec3ui const& );
		static VoxelWorld voxelGenerator6( vec3ui const& );
		static VoxelWorld voxelGenerator7( vec3ui const& );
		static VoxelWorld voxelGenerator8( vec3ui const& );

	private:
		VoxelWorld( vec3ui const& );
	
		vec3ui				size;	// 3D size of the world
		std::vector<bool>	world;	// 3D grid of voxels (true) or empty (false)
};


enum WorldCreationMode {
	MODE_VOXEL_STATIC,
	MODE_VOXEL_PROC,
	MODE_BOXEL
};

class WorldGenerator {
	public:
		class HistoryWorlds {
			public:
				HistoryWorlds( uint32_t max ) : max(max) {};
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

		WorldGenerator( vec3ui const& worldSize, uint32_t maxWorldsStored, WorldCreationMode mode = MODE_VOXEL_STATIC ) : 
			history(maxWorldsStored), 
			worldSize(worldSize),
			mode(mode) {};
		~WorldGenerator( void ) = default;
		WorldGenerator( WorldGenerator const& ) = delete;
		WorldGenerator( WorldGenerator&& ) = delete;
		WorldGenerator& operator=( WorldGenerator const& ) = delete;
		WorldGenerator& operator=( WorldGenerator&& ) = delete;

		void	init( void );
		bool	spawnCloseByWorlds( vec3 const& );
		bool	addeNewWorld( vec2i const& );

		ve::VulkanModel::Builder const&	getBuilder( void ) const noexcept { return builder; };
		ve::VulkanModel::Builder&		getBuilder( void ) noexcept { return builder; };

	private:
		void	fillBufferVoxel1( vec2i const& );
		void	fillBufferVoxel2( vec2i const& );
		void	fillBufferBoxel( vec2i const& );

		HistoryWorlds				history;
		vec3ui						worldSize;
		WorldCreationMode			mode;
		ve::VulkanModel::Builder	builder;
};

float	perlinNoise(float x, float y, ui32 seed);
float	randomNoise(float, float, ui32& seed);

}
