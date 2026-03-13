#include "VoxelWorld.hpp"


namespace vox {

VertexArray getVertexRelativeMonoTexture( vec3 const& relativeOrigin ) {
	VertexArray voxelVertexes;
	for (uint32_t i=0; i<VERTEX_PER_VOXEL; i++) {
		// add 0.5 (half size of a voxel) of every coor so that the position is in the exact center
		voxelVertexes[i].pos.x = VOXEL_VERTEXES[i].pos.x + 0.5f + relativeOrigin.x;
		voxelVertexes[i].pos.y = VOXEL_VERTEXES[i].pos.y + 0.5f + relativeOrigin.y;
		voxelVertexes[i].pos.z = VOXEL_VERTEXES[i].pos.z + 0.5f + relativeOrigin.z;
		voxelVertexes[i].color = VOXEL_VERTEXES[i].color;
		voxelVertexes[i].normal = VOXEL_VERTEXES[i].normal;
		voxelVertexes[i].textureUv = VOXEL_VERTEXES[i].textureUv;
	}
	return voxelVertexes;
}

VertexArray getVertexRelativeAtlasTexture( vec3 const& relativeOrigin ) {
	VertexArray voxelVertexes = getVertexRelativeMonoTexture(relativeOrigin);

	// according to file: textures/texture_dirt_atlas.jpeg
	//  _______________
	// |   | B |   |   |	
	// |___|___|___|___|
	// | L | T | R | B |
	// |___|___|___|___|
	// |   | F |   |   |
	// |___|___|___|___|
	//
	const float W = 1.0f / 4.0f;  // width of a tile
	const float H = 1.0f / 3.0f;  // height of a tile
	const float padding = 0.004f;
	// front face
	voxelVertexes[0].textureUv = vec2{W + padding, 3 * H - padding};
	voxelVertexes[0 + 1].textureUv = vec2{2 * W - padding, 3 * H - padding};
	voxelVertexes[0 + 2].textureUv = vec2{2 * W - padding, 2 * H + padding};
	voxelVertexes[0 + 3].textureUv = vec2{W + padding, 2 * H + padding};
	// back face
	voxelVertexes[4].textureUv = vec2{2 * W - padding, padding};
	voxelVertexes[4 + 1].textureUv = vec2{W + padding, padding};
	voxelVertexes[4 + 2].textureUv = vec2{W + padding, H - padding};
	voxelVertexes[4 + 3].textureUv = vec2{2 * W - padding, H - padding};
	// left face
	voxelVertexes[8].textureUv = vec2{padding, H + padding};
	voxelVertexes[8 + 1].textureUv = vec2{padding, 2 * H - padding};
	voxelVertexes[8 + 2].textureUv = vec2{W - padding, 2 * H - padding};
	voxelVertexes[8 + 3].textureUv = vec2{W - padding, H + padding};
	// right face
	voxelVertexes[12].textureUv = vec2{3 * W - padding, 2 * H - padding};
	voxelVertexes[12 + 1].textureUv = vec2{3 * W - padding, H + padding};
	voxelVertexes[12 + 2].textureUv = vec2{2 * W + padding, H + padding};
	voxelVertexes[12 + 3].textureUv = vec2{2 * W + padding, 2 * H - padding};
	// top face
	voxelVertexes[16].textureUv = vec2{W + padding, 2 * H - padding};
	voxelVertexes[16 + 1].textureUv = vec2{2 * W - padding, 2 * H - padding};
	voxelVertexes[16 + 2].textureUv = vec2{2 * W - padding, H + padding};
	voxelVertexes[16 + 3].textureUv = vec2{W + padding, H + padding};
	// back face
	voxelVertexes[20].textureUv = vec2{3 * W + padding, H + padding};
	voxelVertexes[20 + 1].textureUv = vec2{3 * W + padding, 2 * H - padding};
	voxelVertexes[20 + 2].textureUv = vec2{4 * W - padding, 2 * H - padding};
	voxelVertexes[20 + 3].textureUv = vec2{4 * W - padding, H + padding};
	return voxelVertexes;
}

std::array<uint32_t, INDEX_PER_VOXEL> getIndexRelative( uint32_t start ) {
	std::array<uint32_t, INDEX_PER_VOXEL> indexes;

	for (uint32_t i=0; i<INDEX_PER_VOXEL; i++)
		indexes[i] = start + VOXEL_VERTEX_INDEXES[i];
	return indexes;
}


void WorldGenerator::HistoryWorlds::add(vec2i const& newPos) {
	// drop the oldest position visited if the limit of 
	// the total positions visited is reached
	if (counter.size() == this->max) {
		vec2i const& lastPosInHistory = history.front();
		history.pop_front();

		auto it = counter.find(lastPosInHistory);
		if (--(it->second) == 0)
			counter.erase(it);
		// NB it should also remove the world from GPU and reload model, NBB till an option?
	}
	history.push_back(newPos);
	++counter[newPos];
}

bool WorldGenerator::HistoryWorlds::hasVisited(vec2i const& pos) const {
	return this->counter.find(pos) != this->counter.end();
}

void WorldGenerator::init( vec3 const& playerPos ) {
	this->builder.emptyData();

	vec2i currentWorldPos = this->worldPosFromLocalPos(playerPos);
	this->addeNewWorld(currentWorldPos);
}

bool WorldGenerator::spawnCloseByWorlds( vec3 const& playerPos ) {
	vec2i currentWorldPos = this->worldPosFromLocalPos(playerPos);
	// add a world, if not existent already, in each of these 9 quadrants
	//  __ __ __
	// |NW|N |NE|
	// |__|__|__|
	// | W| M| E|
	// |__|__|__|
	// |SW|S |SE|
	// |__|__|__|
	//
	bool realoadData = false;
	realoadData |= this->addeNewWorld(currentWorldPos);										// M
	realoadData |= this->addeNewWorld(vec2i{currentWorldPos.x, currentWorldPos.y + 1});		// N
	realoadData |= this->addeNewWorld(vec2i{currentWorldPos.x + 1, currentWorldPos.y + 1});	// N-E
	realoadData |= this->addeNewWorld(vec2i{currentWorldPos.x + 1, currentWorldPos.y});		// E
	realoadData |= this->addeNewWorld(vec2i{currentWorldPos.x + 1, currentWorldPos.y - 1});	// S-E
	realoadData |= this->addeNewWorld(vec2i{currentWorldPos.x, currentWorldPos.y - 1});		// S
	realoadData |= this->addeNewWorld(vec2i{currentWorldPos.x - 1, currentWorldPos.y - 1});	// S-W
	realoadData |= this->addeNewWorld(vec2i{currentWorldPos.x - 1, currentWorldPos.y});		// W
	realoadData |= this->addeNewWorld(vec2i{currentWorldPos.x - 1, currentWorldPos.y + 1});	// N-W
	return realoadData;
}

bool WorldGenerator::addeNewWorld( vec2i const& worldPos ) {
	bool newWorldAdded = this->history.hasVisited(worldPos) == false;
	if (newWorldAdded) {
		this->history.add(worldPos);
		this->fillBufferPlainTerrain(worldPos);
	}
	return newWorldAdded;
}

size_t WorldGenerator::getMemoryUsed( void ) const noexcept {
	size_t size = 0U;
	size += this->builder.vertices.size() * sizeof(ve::VulkanModel::Vertex);
	size += this->builder.indices.size() * sizeof(uint32_t);
	return size;
}

void WorldGenerator::fillBufferPlainTerrain( vec2i const& worldPos ) {
	vec3	localPos = this->localPosFromWorldPos(worldPos);
	vec3ui	index(0U);
	// trivial optimization, but at least worldSize^2 voxels are created, could be more
	this->builder.vertices.reserve(this->builder.vertices.size() + this->worldSize.x * this->worldSize.z * VERTEX_PER_VOXEL);
	this->builder.indices.reserve(this->builder.indices.size() + this->worldSize.x * this->worldSize.z * VERTEX_PER_VOXEL);

	for(; index.z<this->worldSize.z; index.z++) {
		for(index.x=0; index.x<this->worldSize.x; index.x++) {
			vec3 centerVoxel{
				static_cast<float>(index.x) + localPos.x,
				static_cast<float>(index.y) + localPos.y,
				static_cast<float>(index.z) + localPos.z,
			};
			
			VertexArray	vertexes = getVertexRelativeAtlasTexture(centerVoxel);
			IndexArray	indexes = getIndexRelative(this->builder.getCurrentIndex());

			this->builder.vertices.insert(this->builder.vertices.end(), vertexes.begin(), vertexes.end());
			this->builder.indices.insert(this->builder.indices.end(), indexes.begin(), indexes.end());
		}
	}
}

vec2i WorldGenerator::worldPosFromLocalPos( vec3 const& localPos ) const noexcept {
	vec2i WorldPos{
		static_cast<int32_t>(localPos.x) / static_cast<int32_t>(this->worldSize.x),
		static_cast<int32_t>(localPos.z) / static_cast<int32_t>(this->worldSize.z)
	};
	if (localPos.x < 0.0f)
		WorldPos.x -= 1;
	if (localPos.z < 0.0f)
		WorldPos.y -= 1;
	return WorldPos;
}

vec3 WorldGenerator::localPosFromWorldPos( vec2i const& worldPos ) const noexcept {
	return vec3{
		static_cast<float>(worldPos.x * static_cast<int32_t>(this->worldSize.x)),
		0.0f,
		static_cast<float>(worldPos.y * static_cast<int32_t>(this->worldSize.z))
	};
}

}	// namespace vox
