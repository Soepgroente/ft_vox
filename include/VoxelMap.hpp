#pragma once

#include "Vectors.hpp"
#include "VoxelChunk.hpp"
#include "World.hpp"

namespace vox {

using ui8 = uint8_t;
using i32 = int32_t;
using ui32 = uint32_t;

enum class Direction : ui8
{
	North,
	East,
	South,
	West
};

class World;
class WorldNavigator;

class VoxelMap
{
	public:

		enum class VoxelType : ui8
		{
			Air = 0,
			Dirt = 1,
			Stone = 2,
			Water = 3,
		};

		VoxelMap();
		~VoxelMap();
		VoxelMap(VoxelMap const&) = delete;
		VoxelMap(VoxelMap&&) = delete;
		VoxelMap& operator=(VoxelMap const&) = delete;
		VoxelMap& operator=(VoxelMap&&) = delete;

		VoxelType*	getChunk(const vec2i& position)	const noexcept;
		ui32	positiveModulo(i32 value, i32 modulus)	const noexcept;

		void	move(Direction direction);
		void	init(WorldNavigator& world);
		vec2i	minPositions;
	
	private:

		VoxelType*	map;
		ui32	worldSeed;
		ui32	chunkSize;
		vec3ui	chunkDimensions;
		i32 	squareSize;
		ui32	totalChunks;
		vec2i	maxPositions;

		std::vector<VoxelChunk>	chunks;

		void	generateChunk(VoxelType* chunkData, const vec2i& chunkPosition);
		void	north();
		void	south();
		void	west();
		void	east();
};

}	// namespace vox
