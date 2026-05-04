#pragma once

#include "ThreadManager.hpp"
#include "Vectors.hpp"
#include "VoxelChunk.hpp"
#include "World.hpp"

namespace vox {

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
		VoxelMap() = delete;
		VoxelMap(ThreadManager& threadManager);
		~VoxelMap() = default;
		VoxelMap(VoxelMap const&) = delete;
		VoxelMap(VoxelMap&&) = delete;
		VoxelMap& operator=(VoxelMap const&) = delete;
		VoxelMap& operator=(VoxelMap&&) = delete;

		bool	update(const vec3& newPosition);
		void	init();

		std::unique_ptr<ve::VulkanModel> createNewModelTerrain( ve::VulkanDevice& device );
		std::unique_ptr<ve::VulkanModel> createNewModelUnderground( ve::VulkanDevice& device );

	private:
		std::vector<VoxelChunk>	map;

		ui32			worldSeed;

		i32 	squareSize;
		vec2i	minPositions;
		vec2i	maxPositions;
		vec2i	playerOnChunk;
		vec3	rawPosition;

		VertexVector	modelVector;
		IndexVector		modelIndexes;

		ThreadManager&	threadManager;

		void	north();
		void	south();
		void	west();
		void	east();

		vec2i	voxelToChunkPosition(const vec3& position) const noexcept;
		void	generateRow(i32 index);
		void	generateColumn(i32 index);

		void	meshRow(i32 index);
		void	meshColumn(i32 index);
		void	setAdjacentPointers();
};

}	// namespace vox
