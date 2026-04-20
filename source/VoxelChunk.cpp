#include "VoxelChunk.hpp"
#include "Config.hpp"
#include "World.hpp"

#include <cassert>
#include <cstring>

namespace vox {

float	perlin(float x, float y, float z);
float	randomNoise(float, float, ui32& seed);

vec3i	VoxelChunk::chunkDimensions = vec3i::zero();
vec3i	VoxelChunk::paddedDimensions = vec3i::zero();
ui32	VoxelChunk::paddedSize = 0;
ui32	VoxelChunk::chunkSize = 0;

VoxelChunk::VoxelChunk(vec2i loc) : location(loc)
{
	map.assign(static_cast<size_t>(paddedSize), VoxelType::Padding);
	vertexes.reserve(chunkDimensions.x * chunkDimensions.z * 8);

	worldPosition = vec3i(chunkDimensions.x * location.width, 0, chunkDimensions.z * location.depth);
}

void	VoxelChunk::generateMap(float seed)
{
	i32 y;
	const i32 waterLevel = Config::seaLevel + 1;
	const i32 dimX = paddedDimensions.x - 1;
	const i32 dimY = paddedDimensions.y - 1;
	const i32 dimZ = paddedDimensions.z - 1;
	const i32 height = chunkDimensions.y;

	for (i32 z = 1; z < dimZ; z++)
	{
		for (i32 x = 1; x < dimX; x++)
		{
			i32 index = this->index(x, 1, z);

			const float worldX = static_cast<float>(worldPosition.width + x - 1);
			const float worldZ = static_cast<float>(worldPosition.depth + z - 1);

			float noiseValue = perlin(worldX * Config::noiseScalar, worldZ * Config::noiseScalar, static_cast<float>(seed));
			i32 heightValue = static_cast<i32>(noiseValue * static_cast<float>(height)) + 1;

			assert(heightValue <= chunkDimensions.height && "height value out of range");
			assert(Config::seaLevel <= chunkDimensions.height && "sea level higher than height of world");

			for (y = 1; y < heightValue; y++)
			{
				map[index] = VoxelType::Dirt;
				index++;
			}
			for (; y < waterLevel; y++)
			{
				map[index] = VoxelType::Water;
				index++;
			}
			for (; y < dimY; y++)
			{
				map[index] = VoxelType::Air;
				index++;
			}
		}
	}
}

void	VoxelChunk::copyAdjacentData()
{
	const VoxelChunk* north = adjacentChunks[static_cast<size_t>(Direction::North)];
	const VoxelChunk* east = adjacentChunks[static_cast<size_t>(Direction::East)];
	const VoxelChunk* south = adjacentChunks[static_cast<size_t>(Direction::South)];
	const VoxelChunk* west = adjacentChunks[static_cast<size_t>(Direction::West)];

	const i32 width = paddedDimensions.x - 1;
	const i32 depth = paddedDimensions.z - 1;
	const i32 copySize = chunkDimensions.y * sizeof(VoxelType);

	if (north != nullptr)
	{
		for (i32 x = 1; x < width; x++)
		{
			i32 src = north->index(x, 1, 1);
			i32 dst = this->index(x, 1, depth);

			std::memcpy(static_cast<void*>(&map[dst]), north->dataAt(src), copySize);
		}
	}
	if (south != nullptr)
	{
		for (i32 x = 1; x < width; x++)
		{
			i32 src = south->index(x, 1, depth - 1);
			i32 dst = this->index(x, 1, 0);

			std::memcpy(&map[dst], south->dataAt(src), copySize);
		}
	}
	if (east != nullptr)
	{
		for (i32 z = 1; z < depth; z++)
		{
			i32 src = east->index(1, 1, z);
			i32 dst = this->index(width, 1, z);

			std::memcpy(&map[dst], east->dataAt(src), copySize);
		}
	}
	if (west != nullptr)
	{
		for (i32 z = 1; z < depth; z++)
		{
			i32 src = west->index(width - 1, 1, z);
			i32 dst = this->index(0, 1, z);

			std::memcpy(&map[dst], west->dataAt(src), copySize);
		}
	}
}

void	VoxelChunk::setAdjacentChunks(VoxelChunk* north, VoxelChunk* east, VoxelChunk* south, VoxelChunk* west) noexcept
{
	adjacentChunks[static_cast<size_t>(Direction::North)] = north;
	adjacentChunks[static_cast<size_t>(Direction::North)] = east;
	adjacentChunks[static_cast<size_t>(Direction::North)] = south;
	adjacentChunks[static_cast<size_t>(Direction::North)] = west;
}

void	VoxelChunk::generateVertexes()
{
	vertexes.clear();

	const i32 widthMax = paddedDimensions.x - 1;
	const i32 dimY = paddedDimensions.y - 1;
	const i32 depthMax = paddedDimensions.z - 1;

	const i32 xStride = paddedDimensions.y;
	const i32 zStride = paddedDimensions.x * paddedDimensions.y;

	const float worldX = static_cast<float>(worldPosition.x);
	const float worldZ = static_cast<float>(worldPosition.z);

	copyAdjacentData();
	for (i32 z = 1; z < depthMax; z++)
	{
		for (i32 x = 1; x < widthMax; x++)
		{
			i32 i = index(x, 1, z);
			for (i32 y = 1; y < dimY; y++, i++)
			{
				if (map[i] == VoxelType::Air)
				{
					continue;
				}

				vec3 world{worldX + static_cast<float>(x - 1), static_cast<float>(y - 1), worldZ + static_cast<float>(z - 1)};

				if (map[i + zStride] == VoxelType::Air)
				{
					addVoxelFace(world, vertexes, static_cast<size_t>(VertexFaces::FRONT));
				}
				if (map[i - zStride] == VoxelType::Air)
				{
					addVoxelFace(world, vertexes, static_cast<size_t>(VertexFaces::BACK));
				}
				if (map[i - xStride] == VoxelType::Air)
				{
					addVoxelFace(world, vertexes, static_cast<size_t>(VertexFaces::LEFT));
				}
				if (map[i + xStride] == VoxelType::Air)
				{
					addVoxelFace(world, vertexes, static_cast<size_t>(VertexFaces::RIGHT));
				}
				if (map[i + 1] == VoxelType::Air)
				{
					addVoxelFace(world, vertexes, static_cast<size_t>(VertexFaces::TOP));
				}
				if (map[i - 1] == VoxelType::Air)
				{
					addVoxelFace(world, vertexes, static_cast<size_t>(VertexFaces::BOTTOM));
				}
			}
		}
	}
}

}	// namespace vox