#include "VoxelMap.hpp"
#include "Config.hpp"
#include "Utils.hpp"

#include <iostream>
#include <algorithm>
#include <stdexcept>
#include <cassert>


namespace vox {
	
float	perlin(float x, float y, float z);
float	randomNoise(float, float, ui32& seed);

using i32 = int32_t;

VoxelMap::VoxelMap(ThreadManager& threadManager) : threadManager(threadManager)
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
	chunksAsVectors.resize(visibleChunks);
	squareSize = visibleWidth;
	totalChunks = visibleChunks;
	minPositions = vec2i{0, 0};
	maxPositions = vec2i{minPositions.x + visibleWidth - 1, minPositions.y + visibleWidth - 1};
	std::cout << "Map ranges from: " << minPositions << " to: " << maxPositions << std::endl;
	worldSeed = 0;
	playerOnChunk = vec2i{minPositions.x + visibleWidth / 2, minPositions.y + visibleWidth / 2};
}

std::unique_ptr<ve::VulkanModel> VoxelMap::createNewModel( ve::VulkanDevice& device ) const {

	std::unique_ptr<ve::VulkanModel> model;

	// std::cout << "Vectors: " << chunksAsVectors.size() << std::endl;


	// for (size_t i = 0; i < chunksAsVectors.size(); i++)
	// {
	// 	std::cout << "Vectors[i] size: " << chunksAsVectors[i].size() << std::endl;
	// }

	model = std::make_unique<ve::VulkanModel>(device, chunksAsVectors, VOXEL_VERTEX_INDEXES);
	return model;
}

void	VoxelMap::init()
{
	vec2i pos = minPositions;
	Stopwatch timer;

	timer.start();
	for (i32 z = 0; z < squareSize; z++)
	{
		for (i32 x = 0; x < squareSize; x++)
		{
			std::cout << "generating chunk: " << pos << "storing it at index: " << getChunkIndex(pos) << std::endl;
			VoxelType* chunkData = getChunk(pos);
			// threadManager.enqueue([this, pos, chunkData] {
			generateChunk(chunkData, pos);
			mapToVertexes(chunkData, chunksAsVectors.at(getChunkIndex(pos)), pos);
			// });
			pos.width += 1;
		}
		pos.width = minPositions.width;
		pos.depth += 1;
	}
	threadManager.waitIdle();
	timer.stop();
	std::cout << "Initial voxel map generation took: " << timer << std::endl;
}

VoxelMap::~VoxelMap()
{
	free(map);
}

VoxelMap::VoxelType* VoxelMap::getChunk(const vec2i& position) const noexcept
{
	return map + getChunkIndex(position) * chunkSize;
}

ui32	VoxelMap::getChunkIndex(const vec2i& position) const noexcept
{
	assert(position.width >= minPositions.width && position.width <= maxPositions.width && "width out of range");
	assert(position.depth >= minPositions.depth && position.depth <= maxPositions.depth && "depth out of range");
	ui32 chunkX = positiveModulo(position.width, squareSize);
	ui32 chunkZ = positiveModulo(position.depth, squareSize);

	std::cout << "pos.x(" << position.x << ") turns into " << chunkX << std::endl;
	std::cout << "pos.y(" << position.y << ") turns into " << chunkZ << std::endl;
	std::cout << "array index: " << chunkZ * squareSize + chunkX << std::endl;
	return chunkZ * squareSize + chunkX;
}

ui32	VoxelMap::positiveModulo(i32 value, i32 modulus) const noexcept
{
	assert(modulus > 0 && "modulus must be positive");
	value = value % modulus;
	if (value < 0)
	{
		return value + modulus;
	}
	return value;
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

vec2i	VoxelMap::voxelToChunkPosition(const vec3& position)
{
	vec2i	chunkPos{
		static_cast<i32>(position.x / chunkDimensions.x),
		static_cast<i32>(position.z / chunkDimensions.z)
	};
	if (position.x < 0.0f)
		chunkPos.width -= 1;
	if (position.z < 0.0f)
		chunkPos.depth -= 1;
	return chunkPos;
}

bool	VoxelMap::update(const vec3& newPosition)
{
	vec2i	moveDirection = voxelToChunkPosition(newPosition) - playerOnChunk;
	
	if (moveDirection == vec2i::zero())
	{
		return false;
	}
	playerOnChunk = playerOnChunk + moveDirection;
	rawPosition = newPosition;
	while (moveDirection.width > 0)
	{
		east();
		moveDirection.width--;
	}
	while (moveDirection.width < 0)
	{
		west();
		moveDirection.width++;
	}
	while (moveDirection.depth < 0)
	{
		south();
		moveDirection.depth++;
	}
	while (moveDirection.depth > 0)
	{
		north();
		moveDirection.depth--;
	}
	// std::cout << "New map limits: " << minPositions << " to " << maxPositions << std::endl;
	assert(minPositions.x + squareSize - 1 == maxPositions.x && "Error: min/max X don't line up");
	assert(minPositions.y + squareSize - 1 == maxPositions.y && "Error: min/max Y don't line up");
	return true;
}

void	VoxelMap::mapToVertexes(VoxelType* data, VoxelChunk& chunk, const vec2i& pos)
{
	chunk.clear();

	ui32 index = 0;

	for (ui32 z = 0; z < chunkDimensions.z; z++)
	{
		for (ui32 x = 0; x < chunkDimensions.x; x++)
		{
			for (ui32 y = 0; y < chunkDimensions.y; y++)
			{
				if (data[index] == VoxelMap::VoxelType::Air ||
					(y < chunkDimensions.y - 1 && data[index + 1] != VoxelMap::VoxelType::Air))
				{
					index++;
					continue;
				}
				vec3 relativePos{
					static_cast<float>(x + pos.width * Config::chunkLength),
					static_cast<float>(y),
					static_cast<float>(z + pos.depth * Config::chunkLength)
				};
				VertexVector voxelVertexes = getVertexRelativeAtlasTexture(relativePos);
				chunk.insert(chunk.end(), voxelVertexes.begin(), voxelVertexes.end());
				index++;
			}
		}
	}
	assert(index == Config::chunkHeight * Config::chunkLength * Config::chunkLength && "oh oh, index is off");
}

/*	From -x (left/west) to +x (right/east) horizontally, y (up/north) to -y (down/south) vertically. */

void	VoxelMap::north()
{
	minPositions.y += 1;
	maxPositions.y += 1;
	vec2i	pos = vec2i{minPositions.x, maxPositions.y};

	for (i32 i = 0; i < squareSize; i++)
	{
		ui32	index = getChunkIndex(pos);
		VoxelType*	ptrToData = map + index * chunkSize;

		generateChunk(ptrToData, pos);
		mapToVertexes(ptrToData, chunksAsVectors.at(index), pos);
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
		ui32	index = getChunkIndex(pos);
		VoxelType*	ptrToData = map + index * chunkSize;
		
		generateChunk(ptrToData, pos);
		mapToVertexes(ptrToData, chunksAsVectors.at(index), pos);
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
		ui32	index = getChunkIndex(pos);
		VoxelType*	ptrToData = map + index * chunkSize;

		generateChunk(ptrToData, pos);
		mapToVertexes(ptrToData, chunksAsVectors.at(index), pos);
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
		ui32	index = getChunkIndex(pos);
		VoxelType*	ptrToData = map + index * chunkSize;
		
		generateChunk(ptrToData, pos);
		mapToVertexes(ptrToData, chunksAsVectors.at(index), pos);
		pos.y += 1;
	}
}

}	// namespace vox
