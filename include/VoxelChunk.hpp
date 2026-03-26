#pragma once

#include "Vectors.hpp"
#include "Vulkan.hpp"

namespace vox {

using ui64 = uint64_t;

class VoxelChunk
{
	public:
		VoxelChunk();
		~VoxelChunk();
		VoxelChunk(VoxelChunk const&) = delete;
		VoxelChunk(VoxelChunk&&) = delete;
		VoxelChunk& operator=(VoxelChunk const&) = delete;
		VoxelChunk& operator=(VoxelChunk&&) = delete;

		bool	mapToChunk(VoxelMap::VoxelType* chunkData, ui64 chunkSize, const vec2i& position);

		ui64	allocated() const noexcept { return capacity * sizeof(ve::VulkanModel::Vertex); }
		ui64	used() const noexcept { return size * sizeof(ve::VulkanModel::Vertex); }

	private:
	
		bool	manageData();

		ve::VulkanModel::Vertex*	data;
		vec2i	position;
		ui64	size;
		ui64	capacity;
};

}	// namespace vox