#include "VoxelMap.hpp"
#include "Config.hpp"

namespace vox {

VoxelMap::VoxelMap()
{
	ui32 visibleChunks = Config::minimumViewingDistance * 4 / (Config::worldSize + 1);

	chunkDimensions = vec3ui{Config::worldSize, Config::worldHeight, Config::worldSize};
	chunkSize = Config::worldSize * Config::worldHeight * Config::worldSize * sizeof(VoxelType);
	map.reserve(chunkSize * visibleChunks);
	chunkIds.reserve(visibleChunks);
}

/*	Explicit for readability for now. 4th position in vec4 contains index for which chunk	*/

VoxelMap::VoxelType&	VoxelMap::operator[](const vec4ui& pos)
{
	assert(	pos.width < chunkDimensions.width &&
			pos.height < chunkDimensions.height &&
			pos.depth < chunkDimensions.depth &&
			"Voxel position out of world");

	ui32 chunkIndex = pos.index * chunkSize;
	ui32 voxelX = pos.width;
	ui32 voxelY = pos.height * chunkDimensions.width;
	ui32 voxelZ = pos.depth * chunkDimensions.width * chunkDimensions.height;

	return map[chunkIndex * chunkSize + voxelY + voxelZ + voxelX];
}

}	// namespace vox