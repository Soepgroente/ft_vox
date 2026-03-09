#pragma once

#include <array>
#include <cstdint>
#include <memory>
#include <map>
#include <iostream>
#include <unordered_map>
#include <deque>

#include "Vulkan.hpp"


namespace vox {

inline constexpr uint32_t	VERTEX_PER_VOXEL = 8U;	// number of vertexes per voxel
inline constexpr uint32_t	INDEX_PER_VOXEL = 36U;	// number of vertex indexes per voxel

inline constexpr std::array<vec3,VERTEX_PER_VOXEL> VOXEL_VERTEXES{
	vec3{-0.5f,  0.5f, -0.5f},	// front-top-left corner
	vec3{ 0.5f,  0.5f, -0.5f},	// front-top-right corner
	vec3{ 0.5f, -0.5f, -0.5f},	// front-bottom-right corner
	vec3{-0.5f, -0.5f, -0.5f},	// front-bottom-left corner
	vec3{-0.5f,  0.5f,  0.5f},	// back-top-left corner
	vec3{ 0.5f,  0.5f,  0.5f},	// back-top-right corner
	vec3{ 0.5f, -0.5f,  0.5f},	// back-bottom-right corner
	vec3{-0.5f, -0.5f,  0.5f},	// back-bottom-left corner
};

std::array<vec3,VERTEX_PER_VOXEL> getVertexRelative( vec3 const& = vec3(0.0f), vec3ui const& = vec3ui(1U) );

inline constexpr std::array<uint32_t,INDEX_PER_VOXEL> VOXEL_VERTEX_INDEXES{
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
	0U, 7U, 4U			// left face
};


class VoxelWorld {
	public:
		~VoxelWorld( void ) = default;
		VoxelWorld( VoxelWorld const& ) = default;
		VoxelWorld( VoxelWorld&& ) = default;
		VoxelWorld& operator=( VoxelWorld const& ) = default;
		VoxelWorld& operator=( VoxelWorld&& ) = default;

		std::vector<bool>::reference		operator[]( vec3ui const& );
		std::vector<bool>::const_reference	operator[]( vec3ui const& ) const;

		vec3ui const&	getSize( void ) const noexcept;
		bool			isVoxel( vec3ui const& ) const;
		// create/remove a single voxel in a 3D pos
		void			setVoxel( vec3ui const&, bool );
		// create/remove a sector of voxels
		void			setVoxel( vec3ui const&, vec3ui const&, bool );
		vec3ui			nextVoxel( vec3ui const& ) const;
		vec3ui			firstVoxel( void ) const;

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

	public:
		WorldGenerator( vec3ui const& worldSize, uint32_t maxWorldsStored, WorldCreationMode mode = MODE_VOXEL_STATIC ) : 
			history(maxWorldsStored), 
			worldSize(worldSize),
			mode(mode) {};
		~WorldGenerator( void ) = default;
		WorldGenerator( WorldGenerator const& ) = default;
		WorldGenerator( WorldGenerator&& ) = default;
		WorldGenerator& operator=( WorldGenerator const& ) = default;
		WorldGenerator& operator=( WorldGenerator&& ) = default;

		void	initGenerator( void );
		bool	spawnCloseByWorlds( vec3 const& );

		ve::VulkanModel::Builder const&	getBuilder( void ) const noexcept { return builder; };
		ve::VulkanModel::Builder&		getBuilder( void ) noexcept { return builder; };

	private:
		bool	addeNewWorld( vec2i const& );
		void	fillBufferVoxel( vec2i const& );
		void	fillBufferBoxel( vec2i const& );

		HistoryWorlds				history;
		vec3ui						worldSize;
		WorldCreationMode			mode;
		ve::VulkanModel::Builder	builder;
};


}
