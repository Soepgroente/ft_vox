#include "VoxelMap.hpp"
#include <iostream>


namespace vox {

/*	From -x (left/west) to +x (right/east) horizontally, y (up/north) to -y (down/south) vertically. */

bool	VoxelMap::update(const vec3& newPosition)
{
	vec2i	moveDirection = voxelToChunkPosition(newPosition) - playerOnChunk;
	Stopwatch timer;

	timer.start();
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
	timer.stop();
	threadManager.waitIdle();
	std::cout << "regeneration took: " << timer << std::endl;
	// std::cout << "New map limits: " << minPositions << " to " << maxPositions << std::endl;
	assert(minPositions.x + squareSize - 1 == maxPositions.x && "Error: min/max X don't line up");
	assert(minPositions.y + squareSize - 1 == maxPositions.y && "Error: min/max Y don't line up");
	return true;
}

void	VoxelMap::meshRow(vec2i pos)
{
	VoxelType*	ptrToData;
	ui32 index;

	for (i32 i = 0; i < squareSize; i++)
	{
		index = getChunkIndex(pos);
		ptrToData = map + index * chunkSize;

		threadManager.enqueue([this, pos, ptrToData, index] {
			mapToVertexes(ptrToData, chunksAsVectors.at(index), pos);
		});
		pos.x += 1;
	}
}

void	VoxelMap::meshColumn(vec2i pos)
{
	VoxelType*	ptrToData;
	ui32 index;

	for (i32 i = 0; i < squareSize; i++)
	{
		index = getChunkIndex(pos);
		ptrToData = map + index * chunkSize;

		threadManager.enqueue([this, pos, ptrToData, index] {
			mapToVertexes(ptrToData, chunksAsVectors.at(index), pos);
		});
		pos.y += 1;
	}
}

void	VoxelMap::generateRow(vec2i pos)
{
	VoxelType*	ptrToData;
	ui32 index;

	for (i32 i = 0; i < squareSize; i++)
	{
		index = getChunkIndex(pos);
		ptrToData = map + index * chunkSize;

		threadManager.enqueue([this, pos, ptrToData] {
			generateChunk(ptrToData, pos);
		});
		pos.x += 1;
	}
}

void	VoxelMap::generateColumn(vec2i pos)
{
	VoxelType*	ptrToData;
	ui32 index;

	for (i32 i = 0; i < squareSize; i++)
	{
		index = getChunkIndex(pos);
		ptrToData = map + index * chunkSize;

		threadManager.enqueue([this, pos, ptrToData] {
			generateChunk(ptrToData, pos);
		});
		pos.y += 1;
	}
}

void	VoxelMap::north()
{
	minPositions.y += 1;
	maxPositions.y += 1;

	vec2i pos = {minPositions.x, maxPositions.y};

	generateRow(pos);
	threadManager.waitIdle();
	meshRow(pos);
	meshRow({pos.x, pos.y - 1});
	threadManager.waitIdle();
}

void	VoxelMap::south()
{
	minPositions.y -= 1;
	maxPositions.y -= 1;

	vec2i	pos = vec2i{minPositions.x, minPositions.y};

	generateRow(pos);
	threadManager.waitIdle();
	meshRow(pos);
	meshRow({pos.x, pos.y + 1});
	threadManager.waitIdle();
}

void	VoxelMap::west()
{
	minPositions.x -= 1;
	maxPositions.x -= 1;

	vec2i	pos = vec2i{minPositions.x, minPositions.y};

	generateColumn(pos);
	threadManager.waitIdle();
	meshColumn(pos);
	meshColumn({pos.x + 1, pos.y});
	threadManager.waitIdle();
}

void	VoxelMap::east()
{
	minPositions.x += 1;
	maxPositions.x += 1;

	vec2i	pos = vec2i{maxPositions.x, minPositions.y};

	generateColumn(pos);
	threadManager.waitIdle();
	meshColumn(pos);
	meshColumn({pos.x - 1, pos.y});
	threadManager.waitIdle();
}

vec3	VoxelMap::getMapMiddle() const noexcept
{
	return vec3((maxPositions.x - minPositions.x) * chunkDimensions.x / 2.0f,
				chunkDimensions.height - 1.0f,
				(maxPositions.y - minPositions.y) * chunkDimensions.z / 2.0f);
}

}	//namespace vox