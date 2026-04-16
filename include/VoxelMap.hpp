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

enum VertexFaces : ui32
{
	FRONT = 0,
	BACK = 4,
	LEFT = 8,
	RIGHT = 12,
	TOP = 16,
	BOTTOM = 20
};

class World;

class VoxelMap
{
	using VertexVector = std::vector<ve::VulkanModel::Vertex>;

	public:

		enum VoxelType : ui8
		{
			Air = 0,
			Dirt = 1,
			Stone = 2,
			Water = 3,
			EmptyButNotAir = 254,
			NoMoreBlocksThisColumn = 255
		};

		VoxelMap() = delete;
		VoxelMap(ThreadManager& threadManager);
		~VoxelMap();
		VoxelMap(VoxelMap const&) = delete;
		VoxelMap(VoxelMap&&) = delete;
		VoxelMap& operator=(VoxelMap const&) = delete;
		VoxelMap& operator=(VoxelMap&&) = delete;

		void	init();
		bool	update(const vec3& newPosition);
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
		std::vector<VertexVector>	terrainVertexes;
		std::vector<VertexVector>	undergroundVertexes;

		PerlinNoiser generator;
		
		VoxelType*	getChunk(const vec2i& position)	const noexcept;
		i32 		getChunkIndex(const vec2i& position) const noexcept;
		vec2i		voxelToChunkPosition(const vec3& position) const noexcept;
		
		void	generateChunk(VoxelType* chunkData, const vec2i& chunkPosition);
		void	mapToVertexes(VoxelType* data, uint32_t indexChunk, const vec2i& pos);
		void	addVisibleFaces(const VoxelType* data, ui32 indexChunk, ui32 voxelChunkPos, const vec3& voxelWorldPos) noexcept;
		void	addVisibleFacesEdges(const VoxelType* data, ui32 indexChunk, ui32 voxelChunkPos, const vec3& voxelWorldPos) noexcept;
		void	addVoxelFace(const vec3& voxelLocation, ui32 indexChunk, ui32 faceIndex, VoxelType voxelType);

		ui32	index(i32 x, i32 y, i32 z) { return static_cast<ui32>(((z * chunkDimensions.x) + x) * chunkDimensions.y + y);}

		void	meshRow(vec2i pos);
		void	meshColumn(vec2i pos);
	
		void	generateRow(vec2i pos);
		void	generateColumn(vec2i pos);

		void	north();
		void	south();
		void	west();
		void	east();
};

}	// namespace vox
