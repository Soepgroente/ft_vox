#pragma once

#include <array>
#include <vector>
#include "VulkanModel.hpp"
#include "Vectors.hpp"

namespace vox {

using ui8 = uint8_t;
using i32 = int32_t;
using ui32 = uint32_t;

enum class VoxelType : ui8
{
	Air = 0,
	Dirt = 1,
	Stone = 2,
	Water = 3,
	NoMoreBlocksThisColumn = 255
};

class VoxelChunk
{
	using VertexVector = std::vector<ve::VulkanModel::Vertex>;

	public:

		VoxelChunk() = delete;
		VoxelChunk(vec2i loc);
		~VoxelChunk() = default;
		VoxelChunk(const VoxelChunk&) = delete;
		VoxelChunk(VoxelChunk&&) = delete;
		VoxelChunk& operator=(const VoxelChunk&) = delete;
		VoxelChunk& operator=(VoxelChunk&&) = delete;
	
		static vec3i	chunkDimensions;
		static ui32		chunkSize;

		void	generateMap(float seed);
		void	generateVertexes();
		i32		index(i32 x, i32 y, i32 z) { return ((z * chunkDimensions.x) + x) * chunkDimensions.y + y; }

	private:
		
		vec2i	location;
		vec3i	worldPosition;
		std::vector<VoxelType>	map;
		VertexVector			vertexes;
		std::array<VoxelChunk*, 4>	adjacentChunks{};
	
		void	northernSquare();
		void	westernSquare();
		void	easternSquare();
		void	southernSquare();
};

}	// namespace vox