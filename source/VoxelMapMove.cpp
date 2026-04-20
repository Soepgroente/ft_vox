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
	while (moveDirection.depth > 0)
	{
		north();
		moveDirection.depth--;
	}
	while (moveDirection.width > 0)
	{
		east();
		moveDirection.width--;
	}
	while (moveDirection.depth < 0)
	{
		south();
		moveDirection.depth++;
	}
	while (moveDirection.width < 0)
	{
		west();
		moveDirection.width++;
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
	for (i32 i = pos.width; i < squareSize; i++)
	{
		map[i].generateVertexes();
	}
}

void	VoxelMap::meshColumn(vec2i pos)
{
	for (i32 i = pos.depth; i < squareSize; i += squareSize)
	{
		map[i].generateVertexes();
	}
}

void	VoxelMap::generateRow(vec2i pos)
{
	for (i32 i = pos.width; i < squareSize; i++)
	{
		map[i].generateMap(worldSeed);
	}
}

void	VoxelMap::generateColumn(vec2i pos)
{
	for (i32 i = pos.depth; i < squareSize; i += squareSize)
	{
		map[i].generateMap(worldSeed);
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
	meshRow(pos);
	meshRow({pos.x, pos.y + 1});
}

void	VoxelMap::west()
{
	minPositions.x -= 1;
	maxPositions.x -= 1;

	vec2i	pos = vec2i{minPositions.x, minPositions.y};

	generateColumn(pos);
	meshColumn(pos);
	meshColumn({pos.x + 1, pos.y});
}

void	VoxelMap::east()
{
	minPositions.x += 1;
	maxPositions.x += 1;

	vec2i	pos = vec2i{maxPositions.x, minPositions.y};

	generateColumn(pos);
	meshColumn(pos);
	meshColumn({pos.x - 1, pos.y});
}

}	//namespace vox