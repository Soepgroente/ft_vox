#include "VoxelChunk.hpp"
#include "Config.hpp"
#include "World.hpp"

#include <cassert>

namespace vox {

float	perlin(float x, float y, float z);
float	randomNoise(float, float, ui32& seed);

vec3i	chunkDimensions = vec3i::zero();
ui32	chunkSize = 0;

VoxelChunk::VoxelChunk(vec2i loc) : location(loc)
{
	map.reserve(VoxelChunk::chunkSize);
	vertexes.reserve(chunkDimensions.x * chunkDimensions.z);

	worldPosition = vec3i(chunkDimensions.x * location.width, 0, chunkDimensions.z * location.depth);
}

void	VoxelChunk::generateMap(float seed)
{
	i32 y = 0;
	i32 dimX = chunkDimensions.x;
	i32 dimY = chunkDimensions.y;
	i32 dimZ = chunkDimensions.z;

	i32 index = 0;

	for (i32 z = 0; z < dimZ; z++)
	{
		for (i32 x = 0; x < dimX; x++)
		{
			float noiseValue = perlin(
				static_cast<float>(static_cast<float>(worldPosition.width + x) * Config::noiseScalar),
				static_cast<float>(static_cast<float>(worldPosition.depth + z) * Config::noiseScalar),
				static_cast<float>(seed));
			i32 heightValue = static_cast<i32>(noiseValue * static_cast<float>(dimY));

			assert(heightValue <= chunkDimensions.height && "height value out of range");
			assert(Config::seaLevel <= chunkDimensions.height && "sea level higher than height of world");
			for (y = 0; y < heightValue; y++)
			{
				map[index] = VoxelType::Dirt;
				index++;
			}
			for (; y < Config::seaLevel; y++)
			{
				map[index] = VoxelType::Water;
				index++;
			}
			for (; y < chunkDimensions.height; y++)
			{
				map[index] = VoxelType::Air;
				index++;
			}
		}
	}
}

void	VoxelChunk::northernSquare()
{

}

void	VoxelChunk::westernSquare()
{
	
}

void	VoxelChunk::easternSquare()
{

}

void	VoxelChunk::southernSquare()
{

}

void	VoxelChunk::generateVertexes()
{
	i32 widthMax = chunkDimensions.x - 1;
	i32 dimY = chunkDimensions.y;
	i32 depthMax = chunkDimensions.z - 1;

	i32 widthMin = 1;
	i32 depthMin = 1;

	if (adjacentChunks.at(static_cast<size_t>(Direction::East)) == nullptr)
	{
		widthMax++;
	}
	else
	{
		easternSquare();
	}
	if (adjacentChunks.at(static_cast<size_t>(Direction::West)) == nullptr)
	{
		widthMin--;
	}
	else
	{
		westernSquare();
	}
	if (adjacentChunks.at(static_cast<size_t>(Direction::North)) == nullptr)
	{
		depthMax++;
	}
	else
	{
		northernSquare();
	}
	if (adjacentChunks.at(static_cast<size_t>(Direction::South)) == nullptr)
	{
		depthMin--;
	}
	else
	{
		southernSquare();
	}
	for (i32 z = depthMin; z < depthMax; z++)
	{
		for (i32 x = widthMin; x < widthMax; x++)
		{
			for (i32 y = 0; y < dimY; y++)
			{
				i32 i = index(x, y, z);

				if (map[i] == VoxelType::Air)
				{
					continue;
				}

				vec3 worldPos{worldPos.x + static_cast<float>(x), static_cast<float>(y), worldPos.z + static_cast<float>(z)};

				if (map[i + z] == VoxelType::Air)
				{
					addVoxelFace(worldPos, vertexes, static_cast<size_t>(VertexFaces::FRONT));
				}
				if (map[i - z] == VoxelType::Air)
				{
					addVoxelFace(worldPos, vertexes, static_cast<size_t>(VertexFaces::BACK));
				}
				if (map[i - x] == VoxelType::Air)
				{
					addVoxelFace(worldPos, vertexes, static_cast<size_t>(VertexFaces::LEFT));
				}
				if (map[i + x] == VoxelType::Air)
				{
					addVoxelFace(worldPos, vertexes, static_cast<size_t>(VertexFaces::RIGHT));
				}
				if (map[i + 1] == VoxelType::Air)
				{
					addVoxelFace(worldPos, vertexes, static_cast<size_t>(VertexFaces::TOP));
				}
				if (map[i - 1] == VoxelType::Air)
				{
					addVoxelFace(worldPos, vertexes, static_cast<size_t>(VertexFaces::BOTTOM));
				}
			}
		}
	}
}

}	// namespace vox