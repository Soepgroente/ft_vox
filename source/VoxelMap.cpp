#include "VoxelMap.hpp"
#include "Config.hpp"

namespace vox {

VoxelMap::VoxelMap()
{
	size = vec3ui(Config::worldSize, Config::worldSize, Config::worldSize);
	map.reserve(size.width * size.height * size.depth);
}

VoxelMap::VoxelMap(const vec3ui& size)
{
	this->size = size;
	map.reserve(size.width * size.height * size.depth);
}

VoxelMap::VoxelType&	VoxelMap::operator[](const vec3ui& pos)
{
	assert(	pos.width < size.width &&
			pos.height < size.height &&
			pos.depth < size.depth &&
			"Voxel position out of world");

	return map[pos.width + pos.height * size.width + pos.depth * size.width * size.height];
}

}	// namespace vox