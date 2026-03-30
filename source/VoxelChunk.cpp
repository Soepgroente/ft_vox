#include "VoxelChunk.hpp"
#include "VoxelMap.hpp"

namespace vox {

// static void insertVoxelVertices(std::vector<ve::VulkanModel::Vertex>& data, const vec3& position)
// {
// 	(void)position;
// 	(void)data;
// 	for (ui32 i = 0; i < VERTEX_PER_VOXEL; i++)
// 	{
// 		;
// 	}
// }

// void	VoxelChunk::mapDataToChunk(void* chunkData, const vec2i& position)
// {
// 	VoxelMap::VoxelType* map = reinterpret_cast<VoxelMap::VoxelType*>(chunkData);
// 	if (data.size() > 0)
// 	{
// 		data.clear();
// 	}
// 	else
// 	{
// 		data.reserve(VOXEL_VERTEXES.size() * Config::chunkLength * Config::chunkHeight * Config::chunkLength / 16);
// 	}
// 	vec3i pos = vec3i(position.width, 0, position.depth);

// 	ui32 width, height, depth;
// 	width = Config::chunkLength;
// 	depth = Config::chunkLength;
// 	height = Config::chunkHeight;

// 	for (ui32 z = 0; z < depth; z++)
// 	{
// 		for (ui32 x = 0; x < width; x++)
// 		{
// 			for (ui32 y = 0; y < height; y++)
// 			{
// 				switch(map[z * width * height + x * height + y])
// 				{
// 					case VoxelMap::VoxelType::Air:
// 						break;
// 					case VoxelMap::VoxelType::Dirt:
// 						break;
// 					case VoxelMap::VoxelType::Stone:
// 						break;
// 					case VoxelMap::VoxelType::Water:
// 						break;
// 					default: break;
// 				}
// 			}
// 		}
// 	}
// }

}	// namespace vox