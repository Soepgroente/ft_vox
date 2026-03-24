#pragma once

#include "Vectors.hpp"

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

class VoxelMap
{
	public:

		enum class VoxelType : ui8 
		{
			Air,
			Dirt,
			Stone,
			Water
		};

		VoxelMap();
		~VoxelMap();
		VoxelMap(VoxelMap const&) = delete;
		VoxelMap(VoxelMap&&) = delete;
		VoxelMap& operator=(VoxelMap const&) = delete;
		VoxelMap& operator=(VoxelMap&&) = delete;

		const VoxelType*	getChunk(const vec2i& position)	const noexcept;
		ui32	positiveModulo(ui32 value, ui32 modulus)	const noexcept;

		void	move(Direction direction);
	
	private:

		VoxelType*	map;
		ui32	chunkSize;
		vec3ui	chunkDimensions;
		ui32 	chunkRowSize;
		ui32	totalChunks;
		vec2i	minPositions;
		vec2i	maxPositions;

		void	generateChunk(VoxelType* chunkData, const vec2i& chunkPosition);
		void	north();
		void	south();
		void	west();
		void	east();
};

}	// namespace vox