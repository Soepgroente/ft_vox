#include "VoxelMap.hpp"
#include "Config.hpp"
#include "Utils.hpp"

#include <iostream>
#include <algorithm>
#include <stdexcept>

namespace vox {

VoxelMap::VoxelMap()
{
	i32 chunks = Config::minimumViewingDistance * 2;
	i32 visibleWidth = chunks / Config::worldSize + 1;
	i32 visibleChunks = visibleWidth * visibleWidth;

	std::cout << "Visible chunks: " << visibleChunks << std::endl;
	chunkDimensions = vec3ui{Config::worldSize, Config::worldHeight, Config::worldSize};
	chunkSize = Config::worldSize * Config::worldHeight * Config::worldSize * sizeof(VoxelType);

	std::cout << "Allocating: " << formatBytes(chunkSize * visibleChunks) << " bytes for voxel map" << std::endl;

	map = reinterpret_cast<VoxelType*>(malloc(chunkSize * visibleChunks));
	if (map == nullptr)
	{
		throw std::runtime_error("Failed to allocate memory for voxel map");
	}
	chunkRowSize = chunks;
	totalChunks = visibleChunks;
	minPositions = vec2i{-visibleWidth / 2, -visibleWidth / 2};
	maxPositions = vec2i{minPositions.x + visibleWidth - 1, minPositions.y + visibleWidth - 1};
}

/*	Explicit for readability for now. 4th position in vec4 contains index for which chunk	*/

VoxelMap::~VoxelMap()
{
	free(map);
}

const VoxelMap::VoxelType* VoxelMap::getChunk(const vec2i& position) const noexcept
{
	ui32 chunkX = positiveModulo(position.x, Config::worldSize);
	ui32 chunkZ = positiveModulo(position.y, Config::worldSize);
	ui32 chunkIndex = chunkZ * chunkRowSize + chunkX;

	return map + chunkIndex * chunkSize;
}

ui32	VoxelMap::positiveModulo(ui32 value, ui32 modulus) const noexcept
{
	return (value % modulus + modulus) % modulus;
}

void	VoxelMap::generateChunk(VoxelType* chunkData, const vec2i& chunkPosition)
{
	(void)chunkPosition;
	std::fill(chunkData, chunkData + chunkSize, VoxelType::Air);
}

void	VoxelMap::move(Direction direction)
{
	switch (direction)
	{
		case Direction::North:
			north();
			break;
		case Direction::South:
			south();
			break;
		case Direction::West:
			west();
			break;
		case Direction::East:
			east();
			break;
	}
}

/*	Mapped like x/y graph in math, from -x (left/west) to +x (right/east) horizontally, +y (up/north) to -y (down/south) vertically. */

void	VoxelMap::north()
{
	minPositions.y += 1;
	maxPositions.y += 1;
	ui32 bottomLeftIndex = totalChunks - chunkRowSize;
	vec2i chunkPos{minPositions.x, maxPositions.y};

	for (ui32 i = bottomLeftIndex; i < totalChunks; i++)
	{
		generateChunk(map + i * chunkSize, chunkPos);
		chunkPos.x++;
	}
}

void	VoxelMap::south()
{
	minPositions.y -= 1;
	maxPositions.y -= 1;
	vec2i chunkPos{minPositions.x, minPositions.y};

	for (ui32 i = 0; i < chunkRowSize; i++)
	{
		generateChunk(map + i * chunkSize, chunkPos);
		chunkPos.x++;
	}
}

void	VoxelMap::west()
{
	minPositions.x -= 1;
	maxPositions.x -= 1;
	vec2i chunkPos{minPositions.x, minPositions.y};

	for (ui32 i = 0; i < totalChunks; i += chunkRowSize)
	{
		generateChunk(map + i * chunkSize, chunkPos);
		chunkPos.y++;
	}
}

void	VoxelMap::east()
{
	minPositions.x += 1;
	maxPositions.x += 1;
	vec2i chunkPos{maxPositions.x, minPositions.y};

	for (ui32 i = chunkRowSize - 1; i < totalChunks; i += chunkRowSize)
	{
		generateChunk(map + i * chunkSize, chunkPos);
		chunkPos.y++;
	}
}

}	// namespace vox