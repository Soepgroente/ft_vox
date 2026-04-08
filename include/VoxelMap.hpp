#pragma once

#include "ThreadManager.hpp"
#include "Vectors.hpp"
// #include "VoxelChunk.hpp"
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

class VoxelMap
{
	using VertexVector = std::vector<ve::VulkanModel::Vertex>;

	public:

		enum class VoxelType : ui8
		{
			Air = 0,
			Dirt = 1,
			Stone = 2,
			Water = 3,
			NoMoreBlocksThisColumn = 255
		};

		VoxelMap() = delete;
		VoxelMap(ThreadManager& threadManager);
		~VoxelMap();
		VoxelMap(VoxelMap const&) = delete;
		VoxelMap(VoxelMap&&) = delete;
		VoxelMap& operator=(VoxelMap const&) = delete;
		VoxelMap& operator=(VoxelMap&&) = delete;

		
		bool	update(const vec3& newPosition);
		void	init();
		vec3	getMapMiddle() const noexcept;
		std::unique_ptr<ve::VulkanModel> createNewModel( ve::VulkanDevice& device ) const;
		
		bool		isReady() const noexcept { return this->ready; }
		VoxelType	getVoxelType(i32 wx, i32 wy, i32 wz) const noexcept;
		
		private:
		
		VoxelType*	map;
		ui32	worldSeed;
		ui32	chunkSize;
		vec3i	chunkDimensions;
		i32 	squareSize;
		ui32	totalChunks;
		vec2i	minPositions;
		vec2i	maxPositions;
		vec2i	playerOnChunk;
		vec3	rawPosition;
		
		bool	ready = false;
		
		ThreadManager&	threadManager;
		std::vector<VertexVector>	chunksAsVectors;
		
		VoxelType*	getChunk(const vec2i& position)	const noexcept;
		i32 		getChunkIndex(const vec2i& position) const noexcept;
		i32		positiveModulo(i32 value, i32 modulus)	const noexcept;
		vec2i	voxelToChunkPosition(const vec3& position) const noexcept;
		bool	isVisible(const vec3& pos) const noexcept;
		bool	localIsVisible(const VoxelType* data, ui32 index) const noexcept;
		void	addEdges(VoxelType* data, VertexVector& chunk, const vec2i& pos);
		void	generateChunk(VoxelType* chunkData, const vec2i& chunkPosition);
		void	mapToVertexes(VoxelType* data, VertexVector& chunk, const vec2i& pos);

		ui32	index(i32 x, i32 y, i32 z) { return static_cast<ui32>(((z * chunkDimensions.x) + x) * chunkDimensions.y + y);}
		void	north();
		void	south();
		void	west();
		void	east();
};

}	// namespace vox
