#pragma once

#include <array>
#include <cstdint>
#include <memory>
#include <map>
#include <iostream>
#include <unordered_map>

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


class WorldGenerator {
	public:
		WorldGenerator( vec3ui const& gridSize ) : _gridSize(gridSize) {};
		~WorldGenerator( void ) = default;
		WorldGenerator( WorldGenerator const& ) = default;
		WorldGenerator( WorldGenerator&& ) = default;
		WorldGenerator& operator=( WorldGenerator const& ) = default;
		WorldGenerator& operator=( WorldGenerator&& ) = default;

		void	initWorld( void );
		bool	checkSurroundings( vec3 const& );
		bool	addeNewGrid( vec2i const& );
		void	fillBufferGrid( vec2i const );
		void	fillBufferGridGreedy( vec2i const );

		ve::VulkanModel::Builder const&	getBuilder( void ) const noexcept { return _builder; };

	private:
		std::unordered_map<vec2i,VoxelGrid>	_world;
		vec3ui								_gridSize;
		ve::VulkanModel::Builder			_builder;
};


}
