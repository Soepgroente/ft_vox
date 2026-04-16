#pragma once

#include "ThreadManager.hpp"
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

class VoxelMap
{
	public:

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
		
		VoxelType	getVoxelType(i32 wx, i32 wy, i32 wz) const noexcept;
		bool		isReady() const noexcept { return this->ready; }
		
		private:
		
		VoxelType*	map;
		ui32	worldSeed;

		i32 	squareSize;
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
		int		visibleFaces(const vec3i& pos) const noexcept;
		int		localVisibleFaces(const VoxelType* data, ui32 index) const noexcept;
		void	addEdges(VoxelType* data, VertexVector& chunk, const vec2i& pos);
		void	generateChunk(VoxelType* chunkData, const vec2i& chunkPosition);
		void	mapToVertexes(VoxelType* data, VertexVector& chunk, const vec2i& pos);
		
		void	north();
		void	south();
		void	west();
		void	east();

		void	generateRow(vec2i pos);
		void	generateColumn(vec2i pos);

		void	meshRow(vec2i pos);
		void	meshColumn(vec2i pos);
};

}	// namespace vox
