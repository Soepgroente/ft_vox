#include "VoxelMap.hpp"
#include "Config.hpp"
#include "Utils.hpp"

#include <iostream>
#include <algorithm>
#include <stdexcept>


namespace vox {
	
float	perlin(float x, float y, float z);
float	randomNoise(float, float, ui32& seed);

using i32 = int32_t;

VoxelMap::VoxelMap()
{
	i32 visibleVoxels = static_cast<i32>(Config::minimumViewingDistance * 2);
	i32 visibleWidth = visibleVoxels / static_cast<i32>(Config::worldSize) + 1;
	i32 visibleChunks = visibleWidth * visibleWidth;

	std::cout << "Visible chunks: " << visibleChunks << std::endl;
	chunkDimensions = vec3ui{Config::worldSize, Config::worldHeight, Config::worldSize};
	chunkSize = Config::worldSize * Config::worldHeight * Config::worldSize;

	std::cout << "Allocating: " << formatBytes(chunkSize * visibleChunks) << " bytes for voxel map" << std::endl;

	map = reinterpret_cast<VoxelType*>(malloc(chunkSize * visibleChunks));
	if (map == nullptr)
	{
		throw std::runtime_error("Failed to allocate memory for voxel map");
	}
	std::cout << "Map is at address: " << map << std::endl;
	chunkRowSize = visibleWidth;
	std::cout << "Size of row: " << chunkRowSize << std::endl;
	totalChunks = visibleChunks;
	minPositions = vec2i{-visibleWidth / 2, -visibleWidth / 2};
	maxPositions = vec2i{minPositions.x + visibleWidth - 1, minPositions.y + visibleWidth - 1};

	std::cout << "Square goes from: " << minPositions << " to " << maxPositions << std::endl;
	worldSeed = 0;

	/*	Generate initial chunks around starting position. */
	
	std::cout << "(CONSTRUCTOR) minPosition: " << minPositions.width << " " << minPositions.depth << std::endl;
}

/*	Explicit for readability for now. 4th position in vec4 contains index for which chunk	*/

void	VoxelMap::init(WorldNavigator& world)
{
	vec2i pos = minPositions;

	for (i32 z = 0; z < (i32)chunkRowSize; z++)
	{
		for (i32 x = 0; x < (i32)chunkRowSize; x++)
		{
			generateChunk((VoxelType*)getChunk(pos), pos);
			world.addNewWorld(vec3i{pos.width, 0, pos.depth});
			pos.width += 1;
		}
		pos.width = minPositions.x;
		pos.depth += 1;
	}
}

VoxelMap::~VoxelMap()
{
	free(map);
}

const VoxelMap::VoxelType* VoxelMap::getChunk(const vec2i& position) const
{
	assert(position.width >= minPositions.width && position.width <= maxPositions.width && "width out of range");
	assert(position.depth >= minPositions.depth && position.depth <= maxPositions.depth && "depth out of range");
	ui32 chunkX = positiveModulo(position.width, chunkRowSize);
	ui32 chunkZ = positiveModulo(position.depth, chunkRowSize);
	
	ui32 chunkIndex = chunkZ * chunkRowSize + chunkX;

	return map + chunkIndex * chunkSize;
}

ui32	VoxelMap::positiveModulo(i32 value, ui32 modulus) const noexcept
{
	return (value % modulus + modulus) % modulus;
}

void	VoxelMap::generateChunk(VoxelType* chunkData, const vec2i& pos)
{
	for (ui32 z = 0; z < chunkDimensions.z; z++)
	{
		for (ui32 x = 0; x < chunkDimensions.x; x++)
		{
			float noiseValue = perlin(
				static_cast<float>((pos.width + x) * Config::noiseScalar),
				static_cast<float>((pos.depth + z) * Config::noiseScalar),
				static_cast<float>(worldSeed));
			float heightValue = (noiseValue + 1.0f) * 0.5f * static_cast<float>(chunkDimensions.y);
			// std::cout << "height: " << heightValue << std::endl;
			for (ui32 y = 0; y < static_cast<ui32>(heightValue); y++)
			{
				if (y < Config::seaLevel)
				{
					chunkData[y * chunkDimensions.z * chunkDimensions.x + z * chunkDimensions.x + x] = VoxelType::Water;
				}
				else
				{
					chunkData[y * chunkDimensions.z * chunkDimensions.x + z * chunkDimensions.x + x] = VoxelType::Dirt;
				}
			}
		}
	}
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
		default: break;
	}
}

/*	Mapped like x/y graph in math, from -x (left/west) to +x (right/east) horizontally, -y (up/north) to +y (down/south) vertically. */

void	VoxelMap::north()
{
	minPositions.y += 1;
	maxPositions.y += 1;
	vec2i	pos = vec2i{minPositions.x, maxPositions.y};
	ui32	index = positiveModulo(pos.y, chunkRowSize) * chunkRowSize;

	for (ui32 i = 0; i < chunkRowSize; i++)
	{
		generateChunk(map + index * chunkSize, pos);
		index += i;
		pos.x += 1;
	}
}

void	VoxelMap::south()
{
	minPositions.y -= 1;
	maxPositions.y -= 1;
	vec2i	pos = vec2i{minPositions.x, minPositions.y};
	ui32	index = positiveModulo(pos.y, chunkRowSize) * chunkRowSize;

	for (ui32 i = 0; i < chunkRowSize; i++)
	{
		generateChunk(map + index * chunkSize, pos);
		index += i;
		pos.x += 1;
	}
}

void	VoxelMap::west()
{
	minPositions.x -= 1;
	maxPositions.x -= 1;
	vec2i	pos = vec2i{minPositions.x, minPositions.y};
	ui32	index = positiveModulo(pos.x, chunkRowSize);

	for (ui32 i = 0; i < chunkRowSize; i++)
	{
		generateChunk(map + index * chunkSize, pos);
		index = (index + chunkRowSize) % chunkSize;
		pos.y += 1;
	}
}

void	VoxelMap::east()
{
	minPositions.x += 1;
	maxPositions.x += 1;
	vec2i	pos = vec2i{maxPositions.x, minPositions.y};
	ui32	index = positiveModulo(pos.x, chunkRowSize);

	for (ui32 i = 0; i < chunkRowSize; i++)
	{
		generateChunk(map + index * chunkSize, pos);
		index = (index + chunkRowSize) % chunkSize;
		pos.y += 1;
	}
}

}	// namespace vox
