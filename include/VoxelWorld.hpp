#pragma once

#include <array>
#include <cstdint>
#include <memory>
#include <iostream>

#include "Vulkan.hpp"


namespace vox {

inline constexpr uint32_t	VERTEX_PER_VOXEL = 8U;	// number of vertexes per voxel
inline constexpr uint32_t	INDEX_PER_VOXEL = 36U;	// number of vertexes per voxel

// 3D rectangular prism with 3 dimensions for each side, is an agglomerate of voxels
class Boxel {
	public:
		constexpr explicit Boxel( vec3ui const& center, vec3ui const& size ) : _center(center), _size(size) {};

		vec3				getCenter( void ) const noexcept; // this is the exact center of the boxel
		vec3				getSize( void ) const noexcept;
		std::vector<vec3>	getVertexes( vec3 const& ) const noexcept;

	private:
		vec3ui	_center;	// the center of the boxel is the coordinate of its front-bottom-left corner
		vec3ui	_size;
};

// 3D cube with fixed edge length (supposed do be 1)
class Voxel {
	public:
		constexpr explicit Voxel( vec3ui const& center) : _center(center) {};

		vec3				getCenter( void ) const noexcept; // this is the exact center of the voxel
		std::vector<vec3>	getVertexes( vec3 const& ) const noexcept;

	private:
		vec3ui	_center;	// the center of the voxel is the coordinate of its front-bottom-left corner
};

inline constexpr std::array<vec3,VERTEX_PER_VOXEL> VOXEL_VERTEXES{
	vec3{-1.0f,  1.0f, -1.0f},	// front-top-left corner
	vec3{ 1.0f,  1.0f, -1.0f},	// front-top-right corner
	vec3{ 1.0f, -1.0f, -1.0f},	// front-bottom-right corner
	vec3{-1.0f, -1.0f, -1.0f},	// front-bottom-left corner
	vec3{-1.0f,  1.0f,  1.0f},	// back-top-left corner
	vec3{ 1.0f,  1.0f,  1.0f},	// back-top-right corner
	vec3{ 1.0f, -1.0f,  1.0f},	// back-bottom-right corner
	vec3{-1.0f, -1.0f,  1.0f},	// back-bottom-left corner
};

inline constexpr std::array<uint32_t,INDEX_PER_VOXEL> VOXEL_VERTEX_INDEXES{
	0, 1, 2,		// front face
	0, 2, 3,		// front face
	4, 7, 6,		// back face
	4, 6, 5,		// back face
	0, 1, 5,		// bottom face
	0, 5, 4,		// bottom face
	3, 7, 6,		// top face
	3, 6, 2,		// top face
	1, 2, 6,		// right face
	1, 6, 5,		// right face
	0, 3, 7,		// left face
	0, 7, 4			// left face
};


class VoxelGrid {
	public:
		VoxelGrid( vec3ui const& );
		~VoxelGrid( void ) = default;
		VoxelGrid( VoxelGrid const& ) = default;
		VoxelGrid( VoxelGrid&& ) = default;
		VoxelGrid& operator=( VoxelGrid const& ) = default;
		VoxelGrid& operator=( VoxelGrid&& ) = default;

		std::vector<bool>::reference		operator[]( vec3ui const& );
		std::vector<bool>::const_reference	operator[]( vec3ui const& ) const;

		vec3ui const&	getSize( void ) const noexcept;
		bool			isVoxel( vec3ui const& ) const;
		void			setVoxel( vec3ui const&, bool );
		void			setVoxel( vec3ui const&, vec3ui const&, bool );
		vec3ui			nextVoxel( vec3ui const& ) const;
		vec3ui			firstVoxel( void ) const;

		std::vector<Voxel>	getVoxels( void );
		// spwans boxels, aka clusters of voxels, more optimized
		std::vector<Boxel>	getBoxels( void );

		static VoxelGrid voxelGenerator1( vec3ui const& );
		static VoxelGrid voxelGenerator2( vec3ui const& );
		static VoxelGrid voxelGenerator3( vec3ui const& );
		static VoxelGrid voxelGenerator4( vec3ui const& );
		static VoxelGrid voxelGenerator5( vec3ui const& );
		static VoxelGrid voxelGenerator6( vec3ui const& );
		static VoxelGrid voxelGenerator7( vec3ui const& );
		static VoxelGrid voxelGenerator8( vec3ui const& );

	private:
		vec3ui				_size;
		std::vector<bool>	_grid;
};

class VoxelWorld {
	public:
		VoxelWorld( vec3ui const& gridSize, bool debugMode = true ) :
			_gridSize(gridSize),
			_grid(gridSize),
			_debugMode(debugMode) {};
		~VoxelWorld( void ) = default;
		VoxelWorld( VoxelWorld const& ) = default;
		VoxelWorld( VoxelWorld&& ) = default;
		VoxelWorld& operator=( VoxelWorld const& ) = default;
		VoxelWorld& operator=( VoxelWorld&& ) = default;

		void						spawnWorld( VoxelGrid (&)( vec3ui const& ) );
		ve::VulkanModel::Builder	generateBufferData( vec3 const&, bool duplicateVertex = false );
		ve::VulkanModel::Builder	generateBufferDataGreedy( vec3 const&, bool duplicateVertex = false );

	private:
		vec3ui		_gridSize;
		VoxelGrid	_grid;
		bool		_debugMode;
};


}
