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
	using VoxelChunk = std::vector<ve::VulkanModel::Vertex>;

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

		VoxelType*	getChunk(const vec2i& position)	const noexcept;
		ui32 		getChunkIndex(const vec2i& position) const noexcept;
		
		bool	update(const vec3& newPosition);
		void	init();
		vec3	getMapMiddle() const noexcept;
		std::unique_ptr<ve::VulkanModel> createNewModel( ve::VulkanDevice& device ) const;

		bool	isReady() const noexcept { return this->ready; }

		
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
		std::vector<VoxelChunk>	chunksAsVectors;

		ui32	positiveModulo(i32 value, i32 modulus)	const noexcept;
		vec2i	voxelToChunkPosition(const vec3& position);
		void	generateChunk(VoxelType* chunkData, const vec2i& chunkPosition);
		void	mapToVertexes(VoxelType* data, VoxelChunk& chunk, const vec2i& pos);
		void	north();
		void	south();
		void	west();
		void	east();
};

}	// namespace vox
