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
inline constexpr float VOXEL_EDGE = 1.0f;
inline constexpr std::array<uint32_t,36> VOXEL_FACES_INDEX{
	0, 1, 2,
	0, 2, 3,
	4, 7, 6,
	4, 6, 5,
	0, 1, 5,
	0, 5, 4,
	3, 7, 6,
	3, 6, 2,
	1, 2, 6,
	1, 6, 5,
	0, 3, 7,
	0, 7, 4,
};

using VoxelGrid = std::array<bool, W_WIDTH * W_LENGTH * W_HEIGHT>;

VoxelGrid generatorVoxTest1( void );
VoxelGrid generatorVoxTest2( void );
VoxelGrid generatorVoxTest3( void );
VoxelGrid generatorVoxTest4( void );

std::vector<vec3>	getVoxelVertexes( uint32_t, uint32_t, uint32_t );
// std::vector<uint32_t>				getVoxelIndexes( unsigned char, uint32_t startIndex = 0);


class VoxelWorld {
	public:
		VoxelWorld( VoxelGrid (&generator)() );
		~VoxelWorld( void ) = default;
		VoxelWorld( VoxelWorld const& ) = default;
		VoxelWorld( VoxelWorld&& ) = default;
		VoxelWorld& operator=( VoxelWorld const& ) = default;
		VoxelWorld& operator=( VoxelWorld&& ) = default;

		void	generateBufferData( void );
		// greedy meshing algorithm
		void	createModelOpt( void ) {};
		bool	isVoxel( uint32_t, uint32_t, uint32_t) const;

		ve::VulkanModel::Builder const&	getBuilder( void ) const noexcept;
		ve::VulkanModel::Builder&		getBuilder( void ) noexcept;

	private:
		VoxelGrid								_grid;
		ve::VulkanModel::Builder				_builder;
};

std::ostream&	operator<<( std::ostream&, VoxelWorld const& );

}
