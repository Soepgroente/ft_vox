#include "VoxelMap.hpp"
#include "Config.hpp"
#include "Utils.hpp"
#include "World.hpp"

#include <iostream>
#include <algorithm>
#include <stdexcept>
#include <cassert>
#include <functional>


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
	chunkDimensions = vec3i{Config::chunkLength, Config::chunkHeight, Config::chunkLength};
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

std::unique_ptr<ve::VulkanModel> VoxelMap::createNewModel( ve::VulkanDevice& device ) const
{
	std::unique_ptr<ve::VulkanModel> model;

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
			VoxelType* chunkData = getChunk(pos);
			threadManager.enqueue([this, pos, chunkData] {
			generateChunk(chunkData, pos);
			});
			pos.width += 1;
		}
		pos.width = minPositions.width;
		pos.depth += 1;
	}
	pos = minPositions;
	threadManager.waitIdle();
	timer.stop();
	std::cout << "Initial chunk generation complete in: " << timer << std::endl;
	timer.reset();
	timer.start();
	for (i32 z = 0; z < squareSize; z++)
	{
		for (i32 x = 0; x < squareSize; x++)
		{
			VoxelType* chunkData = getChunk(pos);
			threadManager.enqueue([this, pos, chunkData] {
			mapToVertexes(chunkData, chunksAsVectors.at(getChunkIndex(pos)), pos);

			});
			pos.width += 1;
		}
		pos.width = minPositions.width;
		pos.depth += 1;
	}
	threadManager.waitIdle();
	timer.stop();
	std::cout << "Initial voxel map generation took: " << timer << std::endl;
	ready = true;
}

VoxelMap::~VoxelMap()
{
	free(map);
}

VoxelMap::VoxelType* VoxelMap::getChunk(const vec2i& position) const noexcept
{
	return map + getChunkIndex(position) * chunkSize;
}

i32	VoxelMap::getChunkIndex(const vec2i& position) const noexcept
{
	assert(position.width >= minPositions.width && position.width <= maxPositions.width && "width out of range");
	assert(position.depth >= minPositions.depth && position.depth <= maxPositions.depth && "depth out of range");
	ui32 chunkX = positiveModulo(position.width, squareSize);
	ui32 chunkZ = positiveModulo(position.depth, squareSize);

	return chunkZ * squareSize + chunkX;
}

i32	VoxelMap::positiveModulo(i32 value, i32 modulus) const noexcept
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
	i32 y = 0;
	float	positionX = static_cast<float>(pos.width * chunkDimensions.width);
	float	positionZ = static_cast<float>(pos.depth * chunkDimensions.depth);

	for (i32 z = 0; z < chunkDimensions.z; z++)
	{
		for (i32 x = 0; x < chunkDimensions.x; x++)
		{
			float noiseValue = perlin(
				static_cast<float>((positionX + static_cast<float>(x)) * Config::noiseScalar),
				static_cast<float>((positionZ + static_cast<float>(z)) * Config::noiseScalar),
				static_cast<float>(worldSeed));
			i32 heightValue = static_cast<i32>(noiseValue * static_cast<float>(chunkDimensions.height));

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

vec2i	VoxelMap::voxelToChunkPosition(const vec3& position) const noexcept
{
	vec2i	chunkPos{
		static_cast<i32>(std::floor(position.x / static_cast<float>(chunkDimensions.x))),
		static_cast<i32>(std::floor(position.z / static_cast<float>(chunkDimensions.z)))
	};
	return chunkPos;
}

VoxelMap::VoxelType	VoxelMap::getVoxelType(i32 x, i32 y, i32 z) const noexcept
{
	if (y < 0 || y >= chunkDimensions.y)
	{
		return VoxelType::Air;
	}

	const i32 width = chunkDimensions.x;
	const i32 depth = chunkDimensions.z;

	
	const i32 chunkX = (x - positiveModulo(x, width)) / width;
	const i32 chunkZ = (z - positiveModulo(z, depth)) / depth;
	const i32 localX = positiveModulo(x, width);
	const i32 localZ = positiveModulo(z, depth);
	
	if (chunkX < minPositions.width || chunkX > maxPositions.width ||
		chunkZ < minPositions.depth || chunkZ > maxPositions.depth)
	{
		return VoxelType::Air;
	}

	const VoxelType* chunk = getChunk(vec2i{chunkX, chunkZ});

	const size_t index =
		(static_cast<size_t>(localZ) * static_cast<size_t>(width) + static_cast<size_t>(localX)) *
			static_cast<size_t>(chunkDimensions.y) +
		static_cast<size_t>(y);

	return chunk[index];
}


bool	VoxelMap::isVisible(const vec3& pos) const noexcept
{
	vec3i p = {
		static_cast<i32>(pos.x),
		static_cast<i32>(pos.y),
		static_cast<i32>(pos.z)
	};

	return	getVoxelType(p.x - 1, p.y, p.z) == VoxelType::Air ||
			getVoxelType(p.x + 1, p.y, p.z) == VoxelType::Air ||
			getVoxelType(p.x, p.y - 1, p.z) == VoxelType::Air ||
			getVoxelType(p.x, p.y + 1, p.z) == VoxelType::Air ||
			getVoxelType(p.x, p.y, p.z - 1) == VoxelType::Air ||
			getVoxelType(p.x, p.y, p.z + 1) == VoxelType::Air;
}

bool	VoxelMap::localIsVisible(const VoxelType* data, ui32 index) const noexcept
{
	const ui32 x = chunkDimensions.y;
	const ui32 z = chunkDimensions.z * chunkDimensions.y;

	if (data[index] == VoxelType::Air)
	{
		return false;
	}
	return	data[index - x] == VoxelType::Air ||
			data[index + x] == VoxelType::Air ||
			data[index - z] == VoxelType::Air ||
			data[index + z] == VoxelType::Air ||
			data[index + 1] == VoxelType::Air ||
			data[index - 1] == VoxelType::Air;
}

void	VoxelMap::addEdges(VoxelType* data, VertexVector& chunk, const vec2i& pos)
{
	const i32 width = chunkDimensions.x;
	const i32 height = chunkDimensions.y;
	const i32 depth = chunkDimensions.z;

	auto emitIfVisible = [&](i32 x, i32 y, i32 z)
	{
		const ui32 i = index(x, y, z);

		if (data[i] == VoxelType::Air)
		{
			return;
		}

		const i32 wx = x + pos.width * static_cast<i32>(Config::chunkLength);
		const i32 wz = z + pos.depth * static_cast<i32>(Config::chunkLength);

		vec3 worldPos{ static_cast<float>(wx), static_cast<float>(y), static_cast<float>(wz) };

		if (isVisible(worldPos) == false)
		{
			return;
		}
		getVertexRelativeAtlasTexture(worldPos, chunk);
	};

	for (i32 z = 0; z < depth; z++)
	{
		for (i32 loopY = 0; loopY < height; loopY++)
		{
			emitIfVisible(0, loopY, z);
			emitIfVisible(width - 1, loopY, z);
		}
	}

	for (i32 x = 1; x < width - 1; x++)
	{
		for (i32 loopY = 0; loopY < height; loopY++)
		{
			emitIfVisible(x, loopY, 0);
			emitIfVisible(x, loopY, depth - 1);
		}
	}

	for (i32 z = 1; z < depth - 1; z++)
	{
		for (i32 x = 1; x < width - 1; x++)
		{
			emitIfVisible(x, 0, z);
			emitIfVisible(x, height - 1, z);
		}
	}	
}

void	VoxelMap::mapToVertexes(VoxelType* data, VertexVector& chunk, const vec2i& pos)
{
	chunk.clear();

	vec3 relativePosition = vec3::zero();

	addEdges(data, chunk, pos);
	for (i32 z = 1; z < chunkDimensions.z - 1; z++)
	{
		relativePosition.z = static_cast<float>(z + pos.depth * static_cast<i32>(Config::chunkLength));
		for (i32 x = 1; x < chunkDimensions.x - 1; x++)
		{
			relativePosition.x = static_cast<float>(x + pos.width * static_cast<i32>(Config::chunkLength));
			for (i32 y = 1; y < chunkDimensions.y - 1; y++)
			{
				relativePosition.y = static_cast<float>(y);
				if (localIsVisible(data, index(x, y, z)) == false)
				{
					continue;
				}
				getVertexRelativeAtlasTexture(relativePosition, chunk);
			}
		}
	}
}

}	// namespace vox
