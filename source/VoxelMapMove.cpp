#include "VoxelMap.hpp"

namespace vox {

/*	From -x (left/west) to +x (right/east) horizontally, y (up/north) to -y (down/south) vertically. */

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

}	//namespace vox