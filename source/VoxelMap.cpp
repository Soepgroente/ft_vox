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

VoxelMap::VoxelMap(ThreadManager& threadManager) : 
	threadManager(threadManager),
	generator{Config::worldSeed, Config::noiseScalar}
{
	i32 visibleVoxels = static_cast<i32>(Config::minimumViewingDistance * 2);
	this->squareSize = visibleVoxels / static_cast<i32>(Config::chunkLength) + 1;
	i32 visibleChunks = this->squareSize * this->squareSize;

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

	terrainVertexes.resize(visibleChunks);
	undergroundVertexes.resize(visibleChunks);
	waterVertexes.resize(visibleChunks);

	minPositions = vec2i{0, 0};
	maxPositions = vec2i{minPositions.x + squareSize - 1, minPositions.y + squareSize - 1};
	std::cout << "Map ranges from: " << minPositions << " to: " << maxPositions << std::endl;
	playerOnChunk = vec2i{minPositions.x + squareSize / 2, minPositions.y + squareSize / 2};
}

VoxelMap::~VoxelMap()
{
	free(map);
}

void	VoxelMap::init()
{
	vec2i pos = minPositions;
	Stopwatch timer;

	timer.start();
	for (i32 z = 0; z < squareSize; z++)
	{
		generateRow(pos);
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
		meshRow(pos);
		pos.depth += 1;
	}
	threadManager.waitIdle();
	timer.stop();
	std::cout << "Initial voxel map generation took: " << timer << std::endl;
	ready = true;
}

std::unique_ptr<ve::VulkanModel> VoxelMap::createNewModelTerrain( ve::VulkanDevice& device ) const
{
	return std::make_unique<ve::VulkanModel>(device, terrainVertexes, VOXEL_VERTEX_INDEXES);
}

std::unique_ptr<ve::VulkanModel> VoxelMap::createNewModelUnderground( ve::VulkanDevice& device ) const
{
	return std::make_unique<ve::VulkanModel>(device, undergroundVertexes, VOXEL_VERTEX_INDEXES);
}

std::unique_ptr<ve::VulkanModel> VoxelMap::createNewModelWater( ve::VulkanDevice& device ) const
{
	return std::make_unique<ve::VulkanModel>(device, waterVertexes, VOXEL_VERTEX_INDEXES);
}

VoxelMap::VoxelType	VoxelMap::getVoxelType(i32 x, i32 y, i32 z) const noexcept
{
	if (y < 0)
	{
		return VoxelType::Dirt;
	}
	if (y >= chunkDimensions.y)
	{
		return VoxelType::Air;
	}

	const i32 width = chunkDimensions.x;
	const i32 depth = chunkDimensions.z;
	const i32 localX = positiveModulo(x, width);
	const i32 localZ = positiveModulo(z, depth);
	const i32 chunkX = (x - localX) / width;
	const i32 chunkZ = (z - localZ) / depth;

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

vec2i	VoxelMap::voxelToChunkPosition(const vec3& position) const noexcept
{
	vec2i	chunkPos{
		static_cast<i32>(std::floor(position.x / static_cast<float>(chunkDimensions.x))),
		static_cast<i32>(std::floor(position.z / static_cast<float>(chunkDimensions.z)))
	};
	return chunkPos;
}

void	VoxelMap::generateChunk(VoxelType* chunkData, const vec2i& pos)
{
	float	positionX = static_cast<float>(pos.width * chunkDimensions.width);
	float	positionZ = static_cast<float>(pos.depth * chunkDimensions.depth);

	for (i32 z = 0; z < chunkDimensions.z; z++)
	{
		for (i32 x = 0; x < chunkDimensions.x; x++)
		{
			float perlinTerrain = static_cast<i32>(this->generator.perlinValue2D(
				positionX + static_cast<float>(x),
				positionZ + static_cast<float>(z)
			));
			i32 heightValue = static_cast<i32>(perlinTerrain * (chunkDimensions.height - 4));

			chunkData[index(x, 0, z)] = VoxelType::Stone;
			for (i32 y = 1; y < chunkDimensions.height; y++)
			{
				ui32 indexChunk = index(x, y, z);
				if (y < heightValue)
				{
					float perlinCave = this->generator.perlinValue3D(
						positionX + static_cast<float>(x),
									static_cast<float>(y),
						positionZ + static_cast<float>(z)
					);
					float t = static_cast<float>(y) / static_cast<float>(chunkDimensions.height);
					float factor = t * t * (3 - 2 * t);
					float treshold = 0.65f + 0.2f * factor;
					if (perlinCave > treshold)	// create cave
					{
						chunkData[indexChunk] = VoxelType::Water;
					}
					else
					{
						chunkData[indexChunk] = VoxelType::Air;
					}
				}
				else if (y < heightValue + 4)
				{
					chunkData[indexChunk] = VoxelType::Dirt;
				}
				else
				{
					chunkData[indexChunk] = VoxelType::Air;
				}
			}
		}
	}
}

void	VoxelMap::mapToVertexes(VoxelType* data, uint32_t indexChunk, const vec2i& pos)
{
	terrainVertexes[indexChunk].clear();
	undergroundVertexes[indexChunk].clear();
	waterVertexes[indexChunk].clear();

	const i32 width = chunkDimensions.x;
	const i32 height = chunkDimensions.y;
	const i32 depth = chunkDimensions.z;

	for (i32 z = 0; z < depth; z++)
	{
		for (i32 x = 0; x < width; x++)
		{
			for (i32 y = 0; y < height; y++)
			{
				ui32 voxelChunkPos = index(x, y, z);
				vec3 voxelWorldPos = {
					static_cast<float>(x + pos.width * Config::chunkLength),
					static_cast<float>(y),
					static_cast<float>(z + pos.depth * Config::chunkLength)
				};

				if ((z == 0 or z == depth - 1) or (x == 0 or x == width - 1) or (y == 0 or y == height - 1))	// edge situation, check surrounding chunks
				{
					addVisibleFacesEdges(data, indexChunk, voxelChunkPos, voxelWorldPos);
				}
				else
				{
					addVisibleFaces(data, indexChunk, voxelChunkPos, voxelWorldPos);
				}
			}
		}
	}
}

void	VoxelMap::addVisibleFacesEdges(const VoxelType* data, ui32 indexChunk, ui32 voxelChunkPos, const vec3& voxelWorldPos) noexcept
{
	if (data[voxelChunkPos] == VoxelType::Air)
	{
		return;
	}

	vec3i	voxelWorldPosInt{
		static_cast<i32>(voxelWorldPos.x),
		static_cast<i32>(voxelWorldPos.y),
		static_cast<i32>(voxelWorldPos.z)
	};

	/*	front, back, left, right, top, bottom faces	*/
	if (getVoxelType(voxelWorldPosInt.x, voxelWorldPosInt.y, voxelWorldPosInt.z + 1) == VoxelType::Air)
	{
		addVoxelFace(voxelWorldPos, indexChunk, static_cast<ui32>(VertexFaces::FRONT), data[voxelChunkPos]);
	}
	if (getVoxelType(voxelWorldPosInt.x, voxelWorldPosInt.y, voxelWorldPosInt.z - 1) == VoxelType::Air)
	{
		addVoxelFace(voxelWorldPos, indexChunk, static_cast<ui32>(VertexFaces::BACK), data[voxelChunkPos]);
	}
	if (getVoxelType(voxelWorldPosInt.x - 1, voxelWorldPosInt.y, voxelWorldPosInt.z) == VoxelType::Air)
	{
		addVoxelFace(voxelWorldPos, indexChunk, static_cast<ui32>(VertexFaces::LEFT), data[voxelChunkPos]);
	}
	if (getVoxelType(voxelWorldPosInt.x + 1, voxelWorldPosInt.y, voxelWorldPosInt.z) == VoxelType::Air)
	{
		addVoxelFace(voxelWorldPos, indexChunk, static_cast<ui32>(VertexFaces::RIGHT), data[voxelChunkPos]);
	}
	if (getVoxelType(voxelWorldPosInt.x, voxelWorldPosInt.y + 1, voxelWorldPosInt.z) == VoxelType::Air)
	{
		addVoxelFace(voxelWorldPos, indexChunk, static_cast<ui32>(VertexFaces::TOP), data[voxelChunkPos]);
	}
	if (getVoxelType(voxelWorldPosInt.x, voxelWorldPosInt.y - 1, voxelWorldPosInt.z) == VoxelType::Air)
	{
		addVoxelFace(voxelWorldPos, indexChunk, static_cast<ui32>(VertexFaces::BOTTOM), data[voxelChunkPos]);
	}
}

void	VoxelMap::addVisibleFaces(const VoxelType* data, ui32 indexChunk, ui32 voxelChunkPos, const vec3& voxelWorldPos) noexcept
{
	if (data[voxelChunkPos] == VoxelType::Air)
	{
		return;
	}

	const ui32 x = chunkDimensions.y;
	const ui32 z = chunkDimensions.x * chunkDimensions.y;

	/*	front, back, left, right, top, bottom faces	*/
	if (data[voxelChunkPos + z] == VoxelType::Air)
	{
		addVoxelFace(voxelWorldPos, indexChunk, static_cast<ui32>(VertexFaces::FRONT), data[voxelChunkPos]);
	}
	if (data[voxelChunkPos - z] == VoxelType::Air)
	{
		addVoxelFace(voxelWorldPos, indexChunk, static_cast<ui32>(VertexFaces::BACK), data[voxelChunkPos]);
	}
	if (data[voxelChunkPos - x] == VoxelType::Air)
	{
		addVoxelFace(voxelWorldPos, indexChunk, static_cast<ui32>(VertexFaces::LEFT), data[voxelChunkPos]);
	}
	if (data[voxelChunkPos + x] == VoxelType::Air)
	{
		addVoxelFace(voxelWorldPos, indexChunk, static_cast<ui32>(VertexFaces::RIGHT), data[voxelChunkPos]);
	}
	if (data[voxelChunkPos + 1] == VoxelType::Air)
	{
		addVoxelFace(voxelWorldPos, indexChunk, static_cast<ui32>(VertexFaces::TOP), data[voxelChunkPos]);
	}
	if (data[voxelChunkPos - 1] == VoxelType::Air)
	{
		addVoxelFace(voxelWorldPos, indexChunk, static_cast<ui32>(VertexFaces::BOTTOM), data[voxelChunkPos]);
	}
}

void	VoxelMap::addVoxelFace(const vec3& voxelWorldPos, ui32 indexChunk, ui32 min, VoxelType voxelType)
{
	ui32 max = min + 4U;

	for (ui32 i = min; i < max; i++)
	{
		if (voxelType == VoxelType::Dirt)
		{
			terrainVertexes[indexChunk].emplace_back
			(
				ve::VulkanModel::Vertex
				{
					vec3
					{
						VOXEL_VERTEXES_ATLAS[i].pos.x + VOXEL_SIZE * 0.5f + voxelWorldPos.x,
						VOXEL_VERTEXES_ATLAS[i].pos.y + VOXEL_SIZE * 0.5f + voxelWorldPos.y,
						VOXEL_VERTEXES_ATLAS[i].pos.z + VOXEL_SIZE * 0.5f + voxelWorldPos.z
					},
				VOXEL_VERTEXES_ATLAS[i].normal,
				VOXEL_VERTEXES_ATLAS[i].textureUv
				}
			);
		}
		else if (voxelType == VoxelType::Water)
		{
			waterVertexes[indexChunk].emplace_back
			(
				ve::VulkanModel::Vertex
				{
					vec3
					{
						VOXEL_VERTEXES[i].pos.x + VOXEL_SIZE * 0.5f + voxelWorldPos.x,
						VOXEL_VERTEXES[i].pos.y + VOXEL_SIZE * 0.5f + voxelWorldPos.y,
						VOXEL_VERTEXES[i].pos.z + VOXEL_SIZE * 0.5f + voxelWorldPos.z
					},
				VOXEL_VERTEXES[i].normal,
				VOXEL_VERTEXES[i].textureUv
				}
			);
		}
		else if (voxelType == VoxelType::Stone)
		{
			undergroundVertexes[indexChunk].emplace_back
			(
				ve::VulkanModel::Vertex
				{
					vec3
					{
						VOXEL_VERTEXES[i].pos.x + VOXEL_SIZE * 0.5f + voxelWorldPos.x,
						VOXEL_VERTEXES[i].pos.y + VOXEL_SIZE * 0.5f + voxelWorldPos.y,
						VOXEL_VERTEXES[i].pos.z + VOXEL_SIZE * 0.5f + voxelWorldPos.z
					},
				VOXEL_VERTEXES[i].normal,
				VOXEL_VERTEXES[i].textureUv
				}
			);
		}
	}
}

}	// namespace vox
