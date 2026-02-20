#pragma once

#include <array>
#include <cstdint>
#include <memory>
#include <iostream>

#include "Vulkan.hpp"


namespace vox {

inline constexpr uint32_t W_WIDTH = 16U;		// 16384U
inline constexpr uint32_t W_LENGTH = 16U;	// 16384U
inline constexpr uint32_t W_HEIGHT = 16U;	// 256U

using VoxelGrid = std::array<bool, W_WIDTH * W_LENGTH * W_HEIGHT>;

VoxelGrid generatorVoxTest1( void );
VoxelGrid generatorVoxTest2( void );
VoxelGrid generatorVoxTest3( void );

class VoxelWorld {
	public:
		static VoxelWorld	createVoxelWorld( VoxelGrid (&generator)() );

		VoxelWorld( void ) = default;
		~VoxelWorld( void ) = default;
		VoxelWorld( VoxelWorld const& ) = default;
		VoxelWorld( VoxelWorld&& ) = default;
		VoxelWorld& operator=( VoxelWorld const& ) = default;
		VoxelWorld& operator=( VoxelWorld&& ) = default;

		// greedy meshing algorithm
		std::shared_ptr<ve::VulkanModel>	generateModel( void );
		bool								hasVoxel( uint32_t, uint32_t, uint32_t) const;
		std::vector<vec3>					generatePositions( void );

	private:
		VoxelGrid	_grid{};
};

std::ostream&	operator<<( std::ostream&, VoxelWorld const& );

}
