#include "VoxelChunk.hpp"
#include "VoxelMap.hpp"

namespace vox {

VoxelChunk::VoxelChunk() : data(nullptr), position(0, 0), size(0), capacity(0)
{
}

VoxelChunk::~VoxelChunk()
{
	free(data);
}

bool	VoxelChunk::manageData()
{
	if (size >= capacity)
	{
		ui64 newCapacity = capacity * 2;
		ve::VulkanModel::Vertex* newData = reinterpret_cast<ve::VulkanModel::Vertex*>(realloc(data, newCapacity * sizeof(ve::VulkanModel::Vertex)));
		if (newData == nullptr)
		{
			return false;
		}
		data = newData;
		capacity = newCapacity;
	}
}

bool	VoxelChunk::mapToChunk(VoxelMap::VoxelType* chunkData, ui64 chunkSize, const vec2i& position)
{
	vec3i pos = vec3i(position.width, 0, position.depth);

	if (size == 0)
	{
		data = reinterpret_cast<ve::VulkanModel::Vertex*>(malloc(chunkSize * sizeof(ve::VulkanModel::Vertex)));
		if (data == nullptr)
		{
			return false;
		}
		capacity = chunkSize;
	}
	size = 0;
	ui32 width, height, depth;
	width = Config::chunkLength;
	depth = Config::chunkLength;
	height = Config::chunkHeight;

	for (ui32 z = 0; z < depth; z++)
	{
		for (ui32 x = 0; x < width; x++)
		{
			for (ui32 y = 0; y < height; y++)
			{
				if (chunkData[z * width * height + x * height + y] != VoxelMap::VoxelType::Air)
				{
					if (!manageData())
					{
						return false;
					}
					data[size] = ve::VulkanModel::Vertex{vec3(pos.x + x, pos.y + y, pos.z + z), vec3(0.0f, 1.0f, 0.0f), vec2(0.0f, 0.0f)};
					size++;
				}
			}
		}
	}
}

}	// namespace vox