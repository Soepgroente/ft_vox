#pragma once

#include "Vox.hpp"

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
		
		vec3ui size;
		std::vector<VoxelType> map;

		VoxelMap();
		VoxelMap(const vec3ui& size);
		~VoxelMap() = default;
		VoxelMap(VoxelMap const&) = delete;
		VoxelMap(VoxelMap&&) = delete;
		VoxelMap& operator=(VoxelMap const&) = delete;
		VoxelMap& operator=(VoxelMap&&) = delete;

		VoxelMap::VoxelType& operator[](const vec3ui& pos);
};

}	// namespace vox