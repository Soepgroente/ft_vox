#pragma once

#include <array>
#include <vector>
#include "VulkanModel.hpp"
#include "Vectors.hpp"

namespace vox {

using ui8 = uint8_t;
using i32 = int32_t;
using ui32 = uint32_t;
using VertexVector = std::vector<ve::VulkanModel::Vertex>;
using IndexVector = std::vector<ui32>;

enum class VoxelType : ui8
{
	Air = 0,
	Dirt = 1,
	Stone = 2,
	Water = 3,
	Padding = 255
};

class VoxelChunk
{
	using VertexVector = std::vector<ve::VulkanModel::Vertex>;

	public:

		VoxelChunk() = delete;
		VoxelChunk(vec2i loc);
		~VoxelChunk() = default;
		VoxelChunk(const VoxelChunk&) = delete;
		VoxelChunk(VoxelChunk&&) noexcept = default;
		VoxelChunk& operator=(VoxelChunk&&) noexcept = default;
		VoxelChunk& operator=(const VoxelChunk&) = delete;
	
		static vec3i	chunkDimensions;
		static vec3i	paddedDimensions;
		static ui32		paddedSize;
		static ui32		chunkSize;

		void	generateMap(float seed);
		void	generateVertexes();

		const VertexVector&	getVertexData() const noexcept { return vertexes; }
		const vec3i getWorldPos() { return this->worldPosition; }
		
		void	setAdjacentChunks(VoxelChunk* north, VoxelChunk* east, VoxelChunk* south, VoxelChunk* west) noexcept;
		void	setLocation(vec2i loc);

		size_t	getVertexSize() const noexcept { return vertexes.size(); }
		i32		index(i32 x, i32 y, i32 z) const noexcept { return ((z * paddedDimensions.x) + x) * paddedDimensions.y + y; }
		VoxelType	at(i32 x, i32 y, i32 z) const noexcept { return map[index(x, y, z)]; }
		VoxelType	at(i32 index) const noexcept { return map[index]; }
		const VoxelType*	dataAt(i32 index) const noexcept { return map.data() + index; }

	private:
		
		vec2i	location;
		vec3i	worldPosition;
		std::vector<VoxelType>	map;
		VertexVector			vertexes;
		std::array<VoxelChunk*, 4>	adjacentChunks{};
	
		void	copyAdjacentData();
};

}	// namespace vox