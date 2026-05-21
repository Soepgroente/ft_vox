#include "VoxelMap.hpp"

#include <iostream>


namespace vox {

/*	From -x (left/west) to +x (right/east) horizontally, y (up/north) to -y (down/south) vertically. */

static i32	floorDivision(i32 a, i32 b)
{
	i32 q = a / b;
	i32 r = a % b;
	if (r != 0 && a < 0)
	{
		q -= 1;
	}
	return q;
}

static vec2i	voxelToChunk(const vec3i& voxel)
{
	return vec2i {
		floorDivision(voxel.x, VoxelChunk::chunkDimensions.x),
		floorDivision(voxel.z, VoxelChunk::chunkDimensions.z)
	};
}

static vec3i roundyRound(const vec3& voxel)
{
	return
	{
		static_cast<i32>(std::floor(voxel.x)),
		static_cast<i32>(std::floor(voxel.y)),
		static_cast<i32>(std::floor(voxel.z))
	};
}

VoxelType	VoxelMap::getVoxelAt(const vec3i& worldVoxel) const noexcept
{
	if (worldVoxel.y <= 0 || worldVoxel.y >= VoxelChunk::chunkDimensions.y)
	{
		return VoxelType::Air;
	}
	vec2i chunk = voxelToChunk(worldVoxel);

	i32 index = (chunk.x - minPositions.x) * squareSize + (chunk.y - minPositions.y);
	if (index < 0 || static_cast<size_t>(index) >= map.size())
	{
		return VoxelType::Air;
	}

	vec3i chunkWorld = map[index].getWorldPos();

	/*	Add one to account for padding	*/
	i32 localX = (worldVoxel.x - chunkWorld.x) + 1;
	i32 localY = (worldVoxel.y - chunkWorld.y) + 1;
	i32 localZ = (worldVoxel.z - chunkWorld.z) + 1;

	if (localX < 1 || localX > VoxelChunk::chunkDimensions.x ||
		localY < 1 || localY > VoxelChunk::chunkDimensions.y ||
		localZ < 1 || localZ > VoxelChunk::chunkDimensions.z)
	{
		return VoxelType::Air;
	}
	return map[index].at(localX, localY, localZ);
}

vec3	VoxelMap::nearestAirVoxel(const vec3i& origin)
{
	return vec3{origin.x, 255, origin.z};
	// const i32 x = origin.x;
	// const i32 y = origin.y;
	// const i32 z = origin.z;
	// const i32 maxRadius = 255;

	// for (i32 r = 1; r <= maxRadius; ++r)
	// {
	// 	for (i32 dz = -r; dz <= r; ++dz)
	// 	for (i32 dy = -r; dy <= r; ++dy)
	// 	for (i32 dx = -r; dx <= r; ++dx)
	// 	{
	// 		const bool onSurface =
	// 			(dx == -r || dx == r) ||
	// 			(dy == -r || dy == r) ||
	// 			(dz == -r || dz == r);

	// 		if (!onSurface)
	// 			continue;

	// 		vec3i v{ x + dx, y + dy, z + dz };
	// 		if (getVoxelAt(v) == VoxelType::Air)
	// 		{
	// 			return vec3{ v.x , v.y , v.z  };
	// 		}
	// 	}
	// }
	// return vec3{origin.x , origin.y , origin.z };
}

void	VoxelMap::insideVoxels(const vec3& position, std::vector<vec3i>& locations) const noexcept
{
	static constexpr float playerRadius = 0.5f * VOXEL_SIZE - epsilon();
	// static const std::array<vec3, 26>	directions {
	// 	vec3{playerRadius, 0.0f, 0.0f},		// right
	// 	vec3{-playerRadius, 0.0f, 0.0f},	// left
	// 	vec3{0.0f, playerRadius, 0.0f},		// up
	// 	vec3{0.0f, -playerRadius, 0.0f},	// down
	// 	vec3{0.0f, 0.0f, playerRadius},		// forward
	// 	vec3{0.0f, 0.0f, -playerRadius},	// backward

	// 	vec3{twoDirections, twoDirections, 0.0f},	// right-up
	// 	vec3{twoDirections, -twoDirections, 0.0f},	// right-down
	// 	vec3{twoDirections, 0.0f, twoDirections},	// right-forward
	// 	vec3{twoDirections, 0.0f, -twoDirections},	// right-back

	// 	vec3{-twoDirections, twoDirections, 0.0f},	// left-up
	// 	vec3{-twoDirections, -twoDirections, 0.0f},	// left-down
	// 	vec3{-twoDirections, 0.0f, twoDirections},	// left-forward
	// 	vec3{-twoDirections, 0.0f, -twoDirections},	// left-back

	// 	vec3{0.0f, twoDirections, twoDirections},	// up-forward
	// 	vec3{0.0f, twoDirections, -twoDirections},	// up-back
	// 	vec3{0.0f, -twoDirections, twoDirections},	// down-forward
	// 	vec3{0.0f, -twoDirections, -twoDirections},	// down-back

	// 	vec3{threeDirections, threeDirections, threeDirections},	// right-up-forward
	// 	vec3{threeDirections, threeDirections, -threeDirections},	// right-up-back
	// 	vec3{threeDirections, -threeDirections, threeDirections},	// right-down-forward
	// 	vec3{threeDirections, -threeDirections, -threeDirections},	// right-down-back
	// 	vec3{-threeDirections, threeDirections, threeDirections},	// left-up-forward
	// 	vec3{-threeDirections, threeDirections, -threeDirections},	// left-up-back
	// 	vec3{-threeDirections, -threeDirections, threeDirections},	// left-down-forward
	// 	vec3{-threeDirections, -threeDirections, -threeDirections},	// left-down-backward
	// };
	
	// for (size_t i = 0; i < locations.size(); i++)
	// {
	// 	locations[i] = roundyRound(position + directions[i]);
	// }
	locations.clear();

	float theta = 0;					// ranges from 0 to PI
	float steptheta = pi() / 10.0f;
	float phi = 0;						// ranges from 0 to 2PI
	float stepPhi = two_pi() / 10.0f;

	for (; theta < pi(); theta += steptheta)
	{
		for (; phi < two_pi(); phi += stepPhi)
		{
			locations.push_back(roundyRound(position + vec3{
				playerRadius * std::sin(theta) * std::cos(phi), 
				playerRadius * std::sin(theta) * std::sin(phi),
				playerRadius * std::cos(theta)}
			));
		}
	}
}

bool	VoxelMap::testVoxels(const vec3& location)
{
	vec2i	chunkPos = voxelToChunk(roundyRound(location));
	i32		index = (chunkPos.x - minPositions.x) * squareSize + (chunkPos.y - minPositions.y);
	vec3i	chunkWorld = map[index].getWorldPos();
	vec3	chunkWorldF = vec3(chunkWorld.x, chunkWorld.y, chunkWorld.z);
	vec3	locationOnChunk = location - chunkWorldF;
	vec3i	locationOnVoxel = roundyRound(locationOnChunk);

	vec3 innerVoxelPos{
		location.x - std::floor(location.x),
		location.y - std::floor(location.y),
		location.z - std::floor(location.z)
	};

	float theta = 0;					// ranges from 0 to PI
	float phi = 0;						// ranges from 0 to 2PI
	float const steptheta = pi() / 10;
	float const stepPhi = two_pi() / 10;
	float const playerRadius = 0.5f * VOXEL_SIZE;

	// std::cout << "\tchecking sphere... " << std::endl;
	// std::cout << "\tchunkPos start: " << chunkPos << std::endl;
	// std::cout << "\tindex start: " << index << std::endl;
	// std::cout << "\tchunkWorld start: " << chunkWorld << std::endl;
	// std::cout << "\tlocationOnVoxel start: " << locationOnVoxel << std::endl;

	for (; theta < pi(); theta += steptheta)
	{
		for (; phi < two_pi(); phi += stepPhi)
		{
			// std::cout << "\t  -----  " << std::endl;
			vec3 checkPos = vec3{
				locationOnChunk.x + playerRadius * std::sin(theta) * std::cos(phi), 
				locationOnChunk.y + playerRadius * std::sin(theta) * std::sin(phi),
				locationOnChunk.z + playerRadius * std::cos(theta)
			};
			vec3i voxelCheckPos = roundyRound(checkPos);
			// NB why this guard is necessary for not breaking everything? 
			if (voxelCheckPos.x < 0 || voxelCheckPos.x >= 16 ||
				voxelCheckPos.y < 0 || voxelCheckPos.y >= 256 ||
				voxelCheckPos.z < 0 || voxelCheckPos.z >= 16)
			{
				continue;
			}
			VoxelType currentVoxelType = map[index].at(voxelCheckPos.x + 1, voxelCheckPos.y + 1, voxelCheckPos.z + 1);
			// std::cout << "\tcheckPos: " << checkPos << std::endl;

			// chunkPos = voxelToChunk(roundyRound(checkPos));
			// std::cout << "\tchunkPos: " << chunkPos << std::endl;
			// index = (chunkPos.x - minPositions.x) * squareSize + (chunkPos.y - minPositions.y);
			// std::cout << "\tindex: " << index << std::endl;
			// chunkWorld = map[index].getWorldPos();
			// std::cout << "\tchunkWorld: " << chunkWorld << std::endl;
			// vec3i checkOnChunk = roundyRound(checkPos - vec3(chunkWorld.x, chunkWorld.y, chunkWorld.z));
			// std::cout << "\tcheckOnChunk: " << checkOnChunk << std::endl;
			// VoxelType currentVoxelType = map[index].at(checkOnChunk.x + 1, checkOnChunk.y + 1, checkOnChunk.z + 1);

			if ((currentVoxelType != VoxelType::Air) and (currentVoxelType != VoxelType::Padding))
			{
				// switch (currentVoxelType)
				// {
				// 	case Dirt:
				// 		std::cout << "\t\ttype: Dirt\n";
				// 		break;
				// 	case Stone:
				// 		std::cout << "\t\ttype: Stone\n";
				// 		break;
				// 	case Water:
				// 		std::cout << "\t\ttype: Water\n";
				// 		break;
				// 	default:
				// 		std::cout << "\t\ttype: orco il dio\n";
				// 		break;
				// }
				std::cout << "\t\tCOLLISION" << std::endl;
				return false;
			}
			// else
			// {
			// 	std::cout << "\t\ttype: Air\n";
			// }
			// std::cout << "\t  -----  "<< std::endl;
		}
	}
	return true;
}

vec3	VoxelMap::detectCollision(const vec3& origin, const vec3& movement)
{
	i32		nSteps = 10;
	float	stepSize = movement.length() / nSteps;
	vec3	movementStep = movement.normalized() * stepSize;
	vec3	position = origin;

	std::cout << "new mov, start: " << origin << std::endl;
	std::cout << "new mov, dest: " << origin + movement << std::endl << std::endl;
	for (i32 i = 0; i < nSteps; i++)
	{
		vec3 nextPosition = position + movementStep;
		std::cout << " - new step, start: " << nextPosition << std::endl;
		if (testVoxels(nextPosition) == false)
		{
			return nextPosition - origin;
		}
		position = nextPosition;
		std::cout << " - new step, end: " << position << std::endl;
	}
	return movement;
}

bool	VoxelMap::update(const vec3& newPosition)
{
	Stopwatch	timer;
	vec2i		delta = voxelToChunkPosition(newPosition) - playerOnChunk;

	if (delta == vec2i::zero())
	{
		return false;
	}
	timer.start();
	playerOnChunk = playerOnChunk + delta;
	minPositions = minPositions + delta;
	maxPositions = maxPositions + delta;
	assert(squareSize >= 2 && "squaresize too small");

	std::cout << "moving by: " << delta << std::endl;

	moveMap(delta);
	setAdjacentPointers();
	enqueueChanges(delta);

	vec2i	pos = minPositions;
	ui32	index = 0;
	for (i32 z = 0; z < squareSize; z++)
	{
		pos.x = minPositions.x;
		for (i32 x = 0; x < squareSize; x++)
		{
			if (scheduledChanges[index] == true)
			{
				VoxelChunk* c = &map[index];
				map[index].setLocation(pos);
				threadManager.enqueue([c] {
					c->generateMap();
				});
			}
			pos.x++;
			index++;
		}
		pos.y++;
	}
	threadManager.waitIdle();
	for (size_t i = 0; i < scheduledChanges.size(); i++)
	{
		if (scheduledChanges[i] == true)
		{
			VoxelChunk* c = &map[i];
			threadManager.enqueue([c] {
				c->generateVertexes();
			});
		}
	}
	threadManager.waitIdle();
	threadManager.enqueue([this] { regenerateTerrainBuffer(); });
	threadManager.enqueue([this] { regenerateUndergroundBuffer(); });
	threadManager.waitIdle();
	timer.stop();
	std::cout << "regeneration took: " << timer << std::endl;
	assert(minPositions.x + squareSize - 1 == maxPositions.x && "Error: min/max X don't line up");
	assert(minPositions.y + squareSize - 1 == maxPositions.y && "Error: min/max Y don't line up");
	return true;
}

void	VoxelMap::enqueueRowChanges(i32 row, std::vector<bool>& scheduled)
{
	i32 index = row * squareSize;

	for (i32 col = 0; col < squareSize; col++)
	{
		scheduled[index] = true;
		index++;
	}
}

void	VoxelMap::enqueueColumnChanges(i32 col, std::vector<bool>& scheduled)
{
	i32 index = col;

	for (i32 row = 0; row < squareSize; row++)
	{
		scheduled[index] = true;
		index += squareSize;
	}
}

void	VoxelMap::enqueueChanges(const vec2i& delta)
{
	const i32 moveEastWest = delta.x;
	const i32 moveNorthSouth = delta.y;

	std::fill(scheduledChanges.begin(), scheduledChanges.end(), false);
	/*	Add all north moves	*/
	for (i32 step = 0; step < moveNorthSouth; step++)
	{
		const i32 bottomRow = squareSize - 1 - step;
		enqueueRowChanges(bottomRow, scheduledChanges);
		enqueueRowChanges(bottomRow - 1, scheduledChanges);
	}
	/*	Add all south moves	*/
	for (i32 step = 0; step < -moveNorthSouth; step++)
	{
		const i32 topRow = step;
		enqueueRowChanges(topRow, scheduledChanges);
		enqueueRowChanges(topRow + 1, scheduledChanges);
	}
	/*	Add all east moves	*/
	for (i32 step = 0; step < moveEastWest; step++)
	{
		const i32 rightCol = squareSize - 1 - step;
		enqueueColumnChanges(rightCol, scheduledChanges);
		enqueueColumnChanges(rightCol - 1, scheduledChanges);
	}
	/*	Add all west moves	*/
	for (i32 step = 0; step < -moveEastWest; step++)
	{
		const i32 leftCol = step;
		enqueueColumnChanges(leftCol, scheduledChanges);
		enqueueColumnChanges(leftCol + 1, scheduledChanges);
	}
}

void	VoxelMap::moveMap(const vec2i& delta)
{
	i32 moveEastWest = delta.x;
	i32 moveNorthSouth = delta.y;

	/*	rotate west	*/
	if (moveEastWest < 0)
	{
		moveEastWest *= -1;
		for (i32 row = 0; row < squareSize; row++)
		{
			auto begin = map.begin() + row * squareSize;
			std::rotate(begin, begin + (squareSize - moveEastWest), begin + squareSize);
		}
	}
	/*	rotate east	*/
	else if (moveEastWest > 0)
	{
		for (i32 row = 0; row < squareSize; row++)
		{
			auto begin = map.begin() + row * squareSize;
			std::rotate(begin, begin + moveEastWest, begin + squareSize);
		}
	}
	/*	rotate south	*/
	if (moveNorthSouth < 0)
	{
		moveNorthSouth *= -1;
		std::rotate(map.begin(), map.end() - squareSize * moveNorthSouth, map.end());
	}
	/*	rotate north	*/
	else if (moveNorthSouth > 0)
	{
		std::rotate(map.begin(), map.begin() + squareSize * moveNorthSouth, map.end());
	}
}

}	//namespace vox
