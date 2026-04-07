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
	return std::make_unique<ve::VulkanModel>(device, chunksAsVectors, VOXEL_VERTEX_INDEXES);
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
			// threadManager.enqueue([this, pos, chunkData] {
			generateChunk(chunkData, pos);
			// });
			pos.width += 1;
		}
		pos.width = minPositions.width;
		pos.depth += 1;
	}
	pos = minPositions;
	std::cout << "Initial chunk generation complete" << std::endl;
	for (i32 z = 0; z < squareSize; z++)
	{
		for (i32 x = 0; x < squareSize; x++)
		{
			VoxelType* chunkData = getChunk(pos);
			// threadManager.enqueue([this, pos, chunkData] {
			mapToVertexes(chunkData, chunksAsVectors.at(getChunkIndex(pos)), pos);

			std::cout << "Generated: " << x << ", " << z << std::endl;
			// });
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

void	VoxelMap::addEdges(VoxelType* data, VoxelChunk& chunk, const vec2i& pos)
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
		VertexVector vertexes = getVertexRelativeAtlasTexture(worldPos);

		chunk.insert(chunk.end(), vertexes.begin(), vertexes.end());
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

void	VoxelMap::mapToVertexes(VoxelType* data, VoxelChunk& chunk, const vec2i& pos)
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
				VertexVector voxelVertexes = getVertexRelativeAtlasTexture(relativePosition);
				chunk.insert(
					chunk.end(),
					std::make_move_iterator(voxelVertexes.begin()),
					std::make_move_iterator(voxelVertexes.end()));
			}
		}
	}
}

/* static VertexVector	getBottomFace(const vec3& location)
{
	constexpr float W = 1.0f / 4.0f;  // width of a tile
	constexpr float H = 1.0f / 3.0f;  // height of a tile
	constexpr float padding = 0.004f;
	VertexVector vertexes(4);
	size_t	bottom = static_cast<size_t>(VertexFaces::BOTTOM);

	for (size_t i = 0; i < 4; i++)
	{
		vertexes[i].pos.x = VOXEL_VERTEXES[bottom + i].pos.x + VOXEL_SIZE * 0.5f + location.x;
		vertexes[i].pos.y = VOXEL_VERTEXES[bottom + i].pos.y + VOXEL_SIZE * 0.5f + location.y;
		vertexes[i].pos.z = VOXEL_VERTEXES[bottom + i].pos.z + VOXEL_SIZE * 0.5f + location.z;
		vertexes[i].normal = vec3::zero();
	}

	vertexes[0].textureUv = vec2{3 * W + padding, H + padding};
	vertexes[0 + 1].textureUv = vec2{3 * W + padding, 2 * H - padding};
	vertexes[0 + 2].textureUv = vec2{4 * W - padding, 2 * H - padding};
	vertexes[0 + 3].textureUv = vec2{4 * W - padding, H + padding};

	return vertexes;
}

static VertexVector	getTopFace(const vec3& location)
{
	constexpr float W = 1.0f / 4.0f;
	constexpr float H = 1.0f / 3.0f;
	constexpr float padding = 0.004f;
	VertexVector vertexes(4);
	size_t	top = static_cast<size_t>(VertexFaces::TOP);

	for (size_t i = 0; i < 4; i++)
	{
		vertexes[i].pos.x = VOXEL_VERTEXES[top + i].pos.x + VOXEL_SIZE * 0.5f + location.x;
		vertexes[i].pos.y = VOXEL_VERTEXES[top + i].pos.y + VOXEL_SIZE * 0.5f + location.y;
		vertexes[i].pos.z = VOXEL_VERTEXES[top + i].pos.z + VOXEL_SIZE * 0.5f + location.z;
		vertexes[i].normal = vec3::zero();
	}

	vertexes[0].textureUv = vec2{W + padding, 2 * H - padding};
	vertexes[0 + 1].textureUv = vec2{2 * W - padding, 2 * H - padding};
	vertexes[0 + 2].textureUv = vec2{2 * W - padding, H + padding};
	vertexes[0 + 3].textureUv = vec2{W + padding, H + padding};

	return vertexes;
}

static void addOtherFaces(VertexVector& vertexes, const vec3& bottomLoc, const vec3& topLoc)
{
	constexpr float W = 1.0f / 4.0f;
	constexpr float H = 1.0f / 3.0f;
	constexpr float padding = 0.004f;
	std::array<size_t, 4>	faces = {
		static_cast<size_t>(VertexFaces::FRONT),
		static_cast<size_t>(VertexFaces::BACK),
		static_cast<size_t>(VertexFaces::LEFT),
		static_cast<size_t>(VertexFaces::RIGHT),
	};
	
	for (size_t face = 0; face < 4; face++)
	{
		VertexVector toAdd(4);
		
		for (size_t i = 0; i < 2; i++)
		{
			toAdd[i].pos.x = VOXEL_VERTEXES[faces[face] + i].pos.x + VOXEL_SIZE * 0.5f + bottomLoc.x;
			toAdd[i].pos.y = VOXEL_VERTEXES[faces[face] + i].pos.y + VOXEL_SIZE * 0.5f + bottomLoc.y;
			toAdd[i].pos.z = VOXEL_VERTEXES[faces[face] + i].pos.z + VOXEL_SIZE * 0.5f + bottomLoc.z;
			toAdd[i].normal = vec3::zero();
		}
		for (size_t i = 0; i < 2; i++)
		{
			toAdd[i].pos.x = VOXEL_VERTEXES[faces[face] + i].pos.x + VOXEL_SIZE * 0.5f + topLoc.x;
			toAdd[i].pos.y = VOXEL_VERTEXES[faces[face] + i].pos.y + VOXEL_SIZE * 0.5f + topLoc.y;
			toAdd[i].pos.z = VOXEL_VERTEXES[faces[face] + i].pos.z + VOXEL_SIZE * 0.5f + topLoc.z;
			toAdd[i].normal = vec3::zero();
		}
		toAdd[0].textureUv = vec2{3 * W + padding, H + padding};
		toAdd[0 + 1].textureUv = vec2{3 * W + padding, 2 * H - padding};
		toAdd[0 + 2].textureUv = vec2{4 * W - padding, 2 * H - padding};
		toAdd[0 + 3].textureUv = vec2{4 * W - padding, H + padding};
		vertexes.insert(
			vertexes.end(),
			std::make_move_iterator(toAdd.begin()),
			std::make_move_iterator(toAdd.end())
		);
	}
}

void	VoxelMap::mapToVertexes(VoxelType* data, VoxelChunk& chunk, const vec2i& pos)
{
	chunk.clear();

	ui32 index = 0;

	for (i32 z = 0; z < chunkDimensions.z; z++)
	{
		for (i32 x = 0; x < chunkDimensions.x; x++)
		{
			for (i32 y = 0; y < chunkDimensions.y; y++)
			{
				if (data[index] == VoxelMap::VoxelType::Air)
				{
					index++;
					continue;
				}
				vec3 bot = {
					x + pos.width * static_cast<i32>(Config::chunkLength),
					y,
					z + pos.depth * static_cast<i32>(Config::chunkLength)
				};
				VertexVector bottomVertexes = getBottomFace(bot);
				chunk.insert(
					chunk.end(),
					std::make_move_iterator(bottomVertexes.begin()),
					std::make_move_iterator(bottomVertexes.end())
				);
				while (y < chunkDimensions.y && data[index] != VoxelMap::VoxelType::Air)
				{
					index++;
					y++;
				}
				vec3 top = {
					x + pos.width * static_cast<i32>(Config::chunkLength),
					y - 1,
					z + pos.depth * static_cast<i32>(Config::chunkLength)
				};
				VertexVector topVertexes = getTopFace(top);
				chunk.insert(
					chunk.end(),
					std::make_move_iterator(topVertexes.begin()),
					std::make_move_iterator(topVertexes.end())
				);

				addOtherFaces(chunk, bot, top);
				// vec3 relativePos{
				// 	static_cast<float>(x + pos.width * static_cast<i32>(Config::chunkLength)),
				// 	static_cast<float>(y),
				// 	static_cast<float>(z + pos.depth * static_cast<i32>(Config::chunkLength))
				// };
				// VertexVector voxelVertexes = getVertexRelativeAtlasTexture(relativePos);
				// chunk.insert(chunk.end(), voxelVertexes.begin(), voxelVertexes.end());
				if (y < chunkDimensions.y)
				{
					index++;
				}
			}
		}
	}
	assert(index == Config::chunkHeight * Config::chunkLength * Config::chunkLength && "oh oh, index is off");
} */

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

vec3	VoxelMap::getMapMiddle() const noexcept
{
	return vec3((maxPositions.x - minPositions.x) * chunkDimensions.x / 2.0f,
				chunkDimensions.height - 1.0f,
				(maxPositions.y - minPositions.y) * chunkDimensions.z / 2.0f);
}

}	// namespace vox
