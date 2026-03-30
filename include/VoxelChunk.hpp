#pragma once

#include "Vectors.hpp"
#include "Vulkan.hpp"

namespace vox {

using ui64 = uint64_t;

class VoxelChunk
{
	public:
		VoxelChunk() = default;
		~VoxelChunk() = default;
		VoxelChunk(VoxelChunk const&) = delete;
		VoxelChunk(VoxelChunk&&) = delete;
		VoxelChunk& operator=(VoxelChunk const&) = delete;
		VoxelChunk& operator=(VoxelChunk&&) = delete;

		void	mapDataToChunk(void* chunkData, const vec2i& position);

	private:
		std::vector<ve::VulkanModel::Vertex>	data;
};

}	// namespace vox