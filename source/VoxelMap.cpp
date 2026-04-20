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

using i32 = int32_t;

VoxelMap::VoxelMap(ThreadManager& threadManager) : threadManager(threadManager)
{
	i32 visibleVoxels = static_cast<i32>(Config::minimumViewingDistance * 2);
	this->squareSize = visibleVoxels / static_cast<i32>(Config::chunkLength) + 1;
	i32 visibleChunks = this->squareSize * this->squareSize;

	std::cout << "Visible chunks: " << visibleChunks << std::endl;
	VoxelChunk::chunkDimensions = vec3i{Config::chunkLength, Config::chunkHeight, Config::chunkLength};
	std::cout << "Chunk dimensions: " << VoxelChunk::chunkDimensions << std::endl;
	VoxelChunk::chunkSize = Config::chunkLength * Config::chunkHeight * Config::chunkLength;
	VoxelChunk::paddedSize = (Config::chunkLength + 2) * (Config::chunkHeight + 2) * (Config::chunkLength + 2);

	std::cout << "Allocating: " << formatBytes(VoxelChunk::chunkSize * visibleChunks * sizeof(VoxelType)) << " for voxel map" << std::endl;

	map.reserve(visibleChunks);
	minPositions = vec2i{0, 0};
	maxPositions = vec2i{minPositions.x + squareSize - 1, minPositions.y + squareSize - 1};
	std::cout << "Map ranges from: " << minPositions << " to: " << maxPositions << std::endl;
	worldSeed = 0;
	playerOnChunk = vec2i{minPositions.x + squareSize / 2, minPositions.y + squareSize / 2};
	rawPosition = vec3::zero();
}

std::unique_ptr<ve::VulkanModel> VoxelMap::createNewModel( ve::VulkanDevice& device ) const
{
	std::unique_ptr<ve::VulkanModel> model;
	size_t totalVertexes = 0;

	for (size_t i = 0; i < map.size(); i++)
	{
		totalVertexes += map[i].getVertexSize();
	}
	VertexVector modelVector;

	modelVector.reserve(totalVertexes);
	for (size_t i = 0; i < map.size(); i++)
	{
		const VertexVector& chunk = map[i].getVertexData();

		modelVector.insert(modelVector.end(), chunk.begin(), chunk.end());
	}

	model = std::make_unique<ve::VulkanModel>(device, modelVector, VOXEL_VERTEX_INDEXES);
	return model;
}

void	VoxelMap::init()
{
	i32 index = 0;
	Stopwatch timer;

	timer.start();

	for (i32 z = 0; z < squareSize; z++)
	{
		for (i32 x = 0; x < squareSize; x++)
		{
			VoxelChunk chunk(vec2i(minPositions.x + x, minPositions.y + z));

			chunk.generateMap(worldSeed);
			map.emplace_back(std::move(chunk));
			index++;
		}
	}
	threadManager.waitIdle();
	timer.stop();
	std::cout << "Initial chunk generation complete in: " << timer << std::endl;
	timer.reset();
	timer.start();
	index = 0;
	for (i32 depth = 0; depth < squareSize; depth++)
	{
		for (i32 width = 0; width < squareSize; width++)
		{
			VoxelChunk* north;
			VoxelChunk* east;
			VoxelChunk* south;
			VoxelChunk* west;

			if (depth == 0)
			{
				south = nullptr;
			}
			else
			{
				south = &map[index - squareSize];
			}
			if (depth == squareSize - 1)
			{
				north = nullptr;
			}
			else
			{
				north = &map[index + squareSize];
			}
			if (width == 0)
			{
				west = nullptr;
			}
			else
			{
				west = &map[index - 1];
			}
			if (width == squareSize - 1)
			{
				east = nullptr;
			}
			else
			{
				east = &map[index + 1];
			}
			map[index].setAdjacentChunks(north, east, south, west);
			index++;
		}
	}
	for (size_t i = 0; i < map.size(); i++)
	{
		map[i].generateVertexes();
	}
	threadManager.waitIdle();
	timer.stop();
	std::cout << "Initial voxel map generation took: " << timer << std::endl;
	ready = true;
}

vec2i	VoxelMap::voxelToChunkPosition(const vec3& position) const noexcept
{
	vec2i	chunkPos{
		static_cast<i32>(std::floor(position.x / static_cast<float>(VoxelChunk::chunkDimensions.x))),
		static_cast<i32>(std::floor(position.z / static_cast<float>(VoxelChunk::chunkDimensions.z)))
	};
	return chunkPos;
}

vec3	VoxelMap::getMapMiddle() const noexcept
{
	return vec3((maxPositions.x + minPositions.x + 1) * VoxelChunk::chunkDimensions.x / 2.0f,
				VoxelChunk::chunkDimensions.height - 1.0f,
				(maxPositions.y + minPositions.y + 1) * VoxelChunk::chunkDimensions.z / 2.0f);
}

}	// namespace vox
