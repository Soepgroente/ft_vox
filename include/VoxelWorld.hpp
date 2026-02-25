#pragma once

#include <array>
#include <cstdint>
#include <memory>
#include <iostream>

#include "Vulkan.hpp"


namespace vox {

inline constexpr uint32_t W_WIDTH = 16U;	// 16384U
inline constexpr uint32_t W_LENGTH = 16U;	// 16384U
inline constexpr uint32_t W_HEIGHT = 16U;	// 256U

inline constexpr uint32_t VERTEX_PER_VOXEL = 8U;	// number of vertexes per voxel
inline constexpr uint32_t INDEX_PER_VOXEL = 36U;	// number of vertexes per voxel

// 3D rectangular prism with 3 dimensions for each side, size=length * VOXEL_EDGE, is an agglomerate of voxels
class Boxel {
	public:
		// are relative to world coordinates: x: left->right, y: down->up, z: from monitor->user
		constexpr explicit Boxel( vec3ui const& center, vec3ui const& size ) : _center(center), _size(size) {};

		vec3				getCenter( void ) const noexcept; // this is the exact center of the boxel
		vec3ui const&		getSize( void ) const noexcept;
		std::vector<vec3>	getVertexes( void ) const noexcept;

	private:
		vec3ui	_center;	// the center of the boxel is the coordinate of its front-bottom-left corner
		vec3ui	_size;
};

// 3D cube with fixed edge length
class Voxel {
	public:
		constexpr Voxel( void ) noexcept = default;
		constexpr explicit Voxel( vec3ui const& center) : _center(center) {};

		vec3				getCenter( void ) const noexcept; // this is the exact center of the voxel
		float				getSize( void ) const noexcept;
		std::vector<vec3>	getVertexes( void ) const noexcept;

		static constexpr float VOXEL_EDGE = 1.0f;

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
	4, 7, 6,
	4, 6, 5,
	0, 1, 5,
	0, 5, 4,
	3, 7, 6,
	3, 6, 2,
	1, 2, 6,
	1, 6, 5,
	0, 3, 7,
	0, 7, 4
};

using VoxelGrid = std::vector<bool>;

VoxelGrid voxelGenerator1( uint32_t, uint32_t, uint32_t );
VoxelGrid voxelGenerator2( uint32_t, uint32_t, uint32_t );
VoxelGrid voxelGenerator3( uint32_t, uint32_t, uint32_t );
VoxelGrid voxelGenerator4( uint32_t, uint32_t, uint32_t );
VoxelGrid voxelGenerator5( uint32_t, uint32_t, uint32_t );

class VoxelWorld {
	public:
		VoxelWorld( vec3ui const& worldSize, bool debugMode = false ) : _worldSize(worldSize), _debugMode(debugMode) {};
		~VoxelWorld( void ) = default;
		VoxelWorld( VoxelWorld const& ) = default;
		VoxelWorld( VoxelWorld&& ) = default;
		VoxelWorld& operator=( VoxelWorld const& ) = default;
		VoxelWorld& operator=( VoxelWorld&& ) = default;

		ve::VulkanModel::Builder	generateBufferData( VoxelGrid (&generator)( uint32_t, uint32_t, uint32_t ), bool duplicateVertex = false );
		ve::VulkanModel::Builder	generateBufferDataGreedy( VoxelGrid (&generator)( uint32_t, uint32_t, uint32_t ), bool duplicateVertex = false );
		
	private:
		// spwans boxels, aka clusters of voxels, more optimized
		std::vector<Boxel>			greedyMeshing( void );

		bool	isVoxel( vec3ui const& ) const;
		bool	isVoxel( uint32_t, uint32_t, uint32_t ) const;
		void	setVoxel( vec3ui const&, bool );
		void	setVoxel( uint32_t, uint32_t, uint32_t, bool );
		void	setVoxel( vec3ui const&, vec3ui const&, bool );
		vec3ui	nextVoxel( vec3ui const& ) const;
		vec3ui	firstVoxel( void ) const;

		vec3ui		_worldSize;
		bool		_debugMode;
		VoxelGrid	_grid;
};


}
