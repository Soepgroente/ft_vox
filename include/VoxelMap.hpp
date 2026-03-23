#pragma once

#include "Vox.hpp"
#include "Vectors.hpp"

namespace vox {

struct VoxelMap
{
	public:

		enum VoxelType
		{
			Air,
			Dirt,
			Stone,
			Water
		};
		
		ui32	chunkSize;
		vec3ui	chunkDimensions;
		std::vector<VoxelType>	map;
		std::vector<ui32>		chunkIds;

		VoxelMap();
		// VoxelMap(const vec3ui& size);
		~VoxelMap() = default;
		VoxelMap(VoxelMap const&) = delete;
		VoxelMap(VoxelMap&&) = delete;
		VoxelMap& operator=(VoxelMap const&) = delete;
		VoxelMap& operator=(VoxelMap&&) = delete;

		VoxelMap::VoxelType& operator[](const vec4ui& pos4);
};

}	// namespace vox