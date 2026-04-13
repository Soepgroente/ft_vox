#pragma once

#include "ThreadManager.hpp"
#include "Vectors.hpp"
#include "World.hpp"
#include "PerlinNoiser.hpp"

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

enum FaceBit : int
{
    FRONT  = 1 << 0,
    BACK   = 1 << 1,
    LEFT   = 1 << 2,
    RIGHT  = 1 << 3,
    TOP    = 1 << 4,
    BOTTOM = 1 << 5
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
		std::unique_ptr<ve::VulkanModel> createNewModelTerrain( ve::VulkanDevice& device ) const;
		std::unique_ptr<ve::VulkanModel> createNewModelUnderground( ve::VulkanDevice& device ) const;
		
		VoxelType	getVoxelType(i32 wx, i32 wy, i32 wz) const noexcept;
		bool		isReady() const noexcept { return this->ready; }
		
		private:
		
		VoxelType*	map;
		ui32	worldSeed;
		ui32	chunkSize;
		vec3i	chunkDimensions;
		i32 	squareSize;
		vec2i	minPositions;
		vec2i	maxPositions;
		vec2i	playerOnChunk;
		vec3	rawPosition;

		bool	ready = false;

		ThreadManager&	threadManager;
		std::vector<VertexVector>	chunksAsVectors;
		std::vector<VertexVector>	terrainVertexes;
		std::vector<VertexVector>	undergroundVertexes;

		PerlinNoiser generator;
		
		VoxelType*	getChunk(const vec2i& position)	const noexcept;
		i32 		getChunkIndex(const vec2i& position) const noexcept;
		i32		positiveModulo(i32 value, i32 modulus)	const noexcept;
		vec2i	voxelToChunkPosition(const vec3& position) const noexcept;
		int		visibleFaces(const vec3i& pos) const noexcept;
		int		localVisibleFaces(const VoxelType* data, ui32 index) const noexcept;
		void	addEdges(VoxelType* data, uint32_t indexChunk, const vec2i& pos);
		void	generateChunk(VoxelType* chunkData, const vec2i& chunkPosition);

		void	mapToVertexes(VoxelType* data, uint32_t indexChunk, const vec2i& pos);
		void	addVertexes(const vec3& position, uint32_t indexChunk, int facesToAdd, VoxelType voxelType);
		void	addVoxelFace(const vec3& voxelLocation, uint32_t indexChunk, size_t faceIndex, VoxelType voxelType);

		ui32	index(i32 x, i32 y, i32 z) { return static_cast<ui32>(((z * chunkDimensions.x) + x) * chunkDimensions.y + y);}
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
