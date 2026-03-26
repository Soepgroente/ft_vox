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
	i32 visibleWidth = visibleVoxels / static_cast<i32>(Config::chunkLength) + 1;
	i32 visibleChunks = visibleWidth * visibleWidth;

	std::cout << "Visible chunks: " << visibleChunks << std::endl;
	chunkDimensions = vec3ui{Config::chunkLength, Config::chunkHeight, Config::chunkLength};
	std::cout << "Chunk dimensions: " << chunkDimensions << std::endl;
	chunkSize = Config::chunkLength * Config::chunkHeight * Config::chunkLength;

	std::cout << "Allocating: " << formatBytes(chunkSize * visibleChunks * sizeof(VoxelType)) << " for voxel map" << std::endl;

	map = reinterpret_cast<VoxelType*>(malloc(chunkSize * visibleChunks * sizeof(VoxelType)));
	if (map == nullptr)
	{
		throw std::runtime_error("Failed to allocate memory for voxel map");
	}
	squareSize = visibleWidth;
	totalChunks = visibleChunks;
	minPositions = vec2i{-visibleWidth / 2, -visibleWidth / 2};
	maxPositions = vec2i{minPositions.x + visibleWidth - 1, minPositions.y + visibleWidth - 1};
	worldSeed = 0;
}

/*	Explicit for readability for now. 4th position in vec4 contains index for which chunk	*/

void	VoxelMap::init(WorldNavigator& world)
{
	vec2i pos = minPositions;

	for (i32 z = 0; z < squareSize; z++)
	{
		for (i32 x = 0; x < squareSize; x++)
		{
			generateChunk((VoxelType*)getChunk(pos), pos);
			pos.width += 1;
		}
		pos.width = minPositions.x;
		pos.depth += 1;
	}
	pos = minPositions;
	for (i32 z = 0; z < squareSize; z++)
	{
		for (i32 x = 0; x < squareSize; x++)
		{
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

VoxelMap::VoxelType* VoxelMap::getChunk(const vec2i& position) const noexcept
{
	assert(position.width >= minPositions.width && position.width <= maxPositions.width && "width out of range");
	assert(position.depth >= minPositions.depth && position.depth <= maxPositions.depth && "depth out of range");
	ui32 chunkX = positiveModulo(position.width, squareSize);
	ui32 chunkZ = positiveModulo(position.depth, squareSize);
	
	ui32 chunkIndex = chunkZ * squareSize + chunkX;

	// std::cout << "Index: " << chunkIndex << std::endl;

	return map + chunkIndex * chunkSize;
}

ui32	VoxelMap::positiveModulo(i32 value, i32 modulus) const noexcept
{
	if (value < 0)
	{
		return (value % modulus) + modulus;
	}
	return value % modulus;
}

void	VoxelMap::generateChunk(VoxelType* chunkData, const vec2i& pos)
{
	ui32 y = 0;

	for (ui32 z = 0; z < chunkDimensions.z; z++)
	{
		for (ui32 x = 0; x < chunkDimensions.x; x++)
		{
			float noiseValue = perlin(
				static_cast<float>((pos.width * chunkDimensions.width + x) * Config::noiseScalar),
				static_cast<float>((pos.depth * chunkDimensions.depth + z) * Config::noiseScalar),
				static_cast<float>(worldSeed));
			noiseValue = std::clamp(noiseValue, -1.0f, 1.0f);
			ui32 heightValue = static_cast<ui32>((noiseValue + 1.0f) * 0.5f * static_cast<float>(chunkDimensions.height));
			// std::cout << "height: " << heightValue << std::endl;
			assert(heightValue <= chunkDimensions.height && "height value out of range");
			assert(Config::seaLevel <= chunkDimensions.height && "sea level higher than height of world");
			for (y = 0; y < heightValue; y++)
			{
				chunkData[z * chunkDimensions.y * chunkDimensions.x + x * chunkDimensions.y + y] = VoxelType::Dirt;
			}
			for (; y < Config::seaLevel; y++)
			{
				chunkData[z * chunkDimensions.y * chunkDimensions.x + x * chunkDimensions.y + y] = VoxelType::Water;
			}
			for (; y < chunkDimensions.height; y++)
			{
				chunkData[z * chunkDimensions.y * chunkDimensions.x + x * chunkDimensions.y + y] = VoxelType::Air;
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

	for (i32 i = 0; i < squareSize; i++)
	{
		generateChunk(getChunk(pos), pos);
		pos.x += 1;
	}
}

void	VoxelMap::south()
{
	minPositions.y -= 1;
	maxPositions.y -= 1;
	vec2i	pos = vec2i{minPositions.x, minPositions.y};

	for (i32 i = 0; i < squareSize; i++)
	{
		generateChunk(getChunk(pos), pos);
		pos.x += 1;
	}
}

void	VoxelMap::west()
{
	minPositions.x -= 1;
	maxPositions.x -= 1;
	vec2i	pos = vec2i{minPositions.x, minPositions.y};

	for (i32 i = 0; i < squareSize; i++)
	{
		generateChunk(getChunk(pos), pos);
		pos.y += 1;
	}
}

void	VoxelMap::east()
{
	minPositions.x += 1;
	maxPositions.x += 1;
	vec2i	pos = vec2i{maxPositions.x, minPositions.y};

	for (i32 i = 0; i < squareSize; i++)
	{
		generateChunk(getChunk(pos), pos);
		pos.y += 1;
	}
}

}	// namespace vox
