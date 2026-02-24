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
inline constexpr uint32_t SIZE_VOXEL = 8U;	// number of vertexes per voxel

class Boxel {
	public:
		explicit Boxel( vec3 const& pos, vec3 const& size );

		vec3 const&			getCenter( void ) const noexcept;
		vec3				getSize( void ) const noexcept;
		std::vector<vec3>	getVertexes( void ) const noexcept;

	private:
		vec3	center;
		float	lengthX;
		float	lengthY;
		float	lengthZ;
};

class Voxel {
	public:
		explicit Voxel( vec3 const& pos);

		vec3 const&			getCenter( void ) const noexcept;
		std::vector<vec3>	getVertexes( void ) const noexcept;

		static constexpr float VOXEL_EDGE = 1.0f;

	private:
		vec3	center;
};

inline constexpr std::array<vec3,SIZE_VOXEL> VOXEL_VERTEXES{
	vec3{-1.0f,  1.0f, -1.0f},	// front top left
	vec3{ 1.0f,  1.0f, -1.0f},	// front top right
	vec3{ 1.0f, -1.0f, -1.0f},	// front bottom right
	vec3{-1.0f, -1.0f, -1.0f},	// front bottom left
	vec3{-1.0f,  1.0f,  1.0f},	// back top left
	vec3{ 1.0f,  1.0f,  1.0f},	// back top right
	vec3{ 1.0f, -1.0f,  1.0f},	// back bottom right
	vec3{-1.0f, -1.0f,  1.0f},	// back bottom left
};

inline constexpr std::array<uint32_t,36> VOXEL_FACES{
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

std::vector<bool> generatorVoxTest1( uint32_t, uint32_t, uint32_t );
std::vector<bool> generatorVoxTest2( uint32_t, uint32_t, uint32_t );
std::vector<bool> generatorVoxTest3( uint32_t, uint32_t, uint32_t );
std::vector<bool> generatorVoxTest4( uint32_t, uint32_t, uint32_t );

class VoxelWorld {
	public:
		VoxelWorld( std::vector<bool> (&generator)(uint32_t, uint32_t, uint32_t) );
		~VoxelWorld( void ) = default;
		VoxelWorld( VoxelWorld const& ) = default;
		VoxelWorld( VoxelWorld&& ) = default;
		VoxelWorld& operator=( VoxelWorld const& ) = default;
		VoxelWorld& operator=( VoxelWorld&& ) = default;

		void	generateBufferData( void );
		void	generateBufferDataOpt( void );

		ve::VulkanModel::Builder const&	getBuilder( void ) const noexcept;
		ve::VulkanModel::Builder&		getBuilder( void ) noexcept;

	private:
		std::vector<Voxel>			_voxels;
		std::vector<Boxel>			_boxels;
		ve::VulkanModel::Builder	_builder;
};


}
