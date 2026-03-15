#include "VoxelWorld.hpp"
#include "Utils.hpp"


namespace vox {

VertexVector getVertexRelativeMonoTexture( vec3 const& relativeOrigin ) {
	VertexVector voxelVertexes(VERTEX_PER_VOXEL);
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

VertexVector getVertexRelativeAtlasTexture( vec3 const& relativeOrigin ) {
	VertexVector voxelVertexes = getVertexRelativeMonoTexture(relativeOrigin);

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

IndexVector getIndexRelative( uint32_t start ) {
	IndexVector indexes(INDEX_PER_VOXEL);

	for (uint32_t i=0; i<INDEX_PER_VOXEL; i++)
		indexes[i] = start + VOXEL_VERTEX_INDEXES[i];
	return indexes;
}

// NB initialize with relative pos (not v3)
World::World( vec3ui const& worldSize, vec3 const& localPos ) {
	vec3ui index(0U);
	for(; index.z<worldSize.z; index.z++) {
		for(index.x=0; index.x<worldSize.x; index.x++) {
			vec3 centerVoxel{
				static_cast<float>(index.x) + localPos.x,
				static_cast<float>(index.y) + localPos.y,
				static_cast<float>(index.z) + localPos.z,
			};
			VertexVector voxelVertexes = getVertexRelativeAtlasTexture(centerVoxel);
			this->vertexes.insert(this->vertexes.end(), voxelVertexes.begin(), voxelVertexes.end());
		}
	}
}

void WorldGenerator::init( vec3 const& start ) {
	this->currentWorldPos = vec2i{-100, -100};
	this->spawnCloseByWorlds(start);
}

bool WorldGenerator::spawnCloseByWorlds( vec3 const& start ) {
	vec2i playerPos = this->worldPosFromLocalPos(start);
	if (playerPos == this->currentWorldPos)
		return false;
	this->currentWorldPos = playerPos;
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
	realoadData |= this->addeNewWorld(vec2i{playerPos.x - 1, playerPos.y - 1});
	realoadData |= this->addeNewWorld(vec2i{playerPos.x, playerPos.y - 1});
	realoadData |= this->addeNewWorld(vec2i{playerPos.x + 1, playerPos.y - 1});
	realoadData |= this->addeNewWorld(vec2i{playerPos.x - 1, playerPos.y});
	realoadData |= this->addeNewWorld(playerPos);
	realoadData |= this->addeNewWorld(vec2i{playerPos.x + 1, playerPos.y});
	realoadData |= this->addeNewWorld(vec2i{playerPos.x - 1, playerPos.y + 1});
	realoadData |= this->addeNewWorld(vec2i{playerPos.x, playerPos.y + 1});
	realoadData |= this->addeNewWorld(vec2i{playerPos.x + 1, playerPos.y + 1});
	return realoadData;
}

size_t WorldGenerator::getMemoryUsed( void ) noexcept {
	size_t size = 0U;
	size += this->totVoxels * VERTEX_PER_VOXEL * sizeof(ve::VulkanModel::Vertex);
	size += this->totVoxels * INDEX_PER_VOXEL * sizeof(uint32_t);
	return size;
}

std::unique_ptr<ve::VulkanModel> WorldGenerator::createNewModel( ve::VulkanDevice& device ) {
	std::vector<VertexVector*>	vertexes(this->worlds.size());
	uint32_t i = 0U;
	for (auto const& [pos , _] : this->worlds)
		vertexes[i++] = &this->worlds[pos].getVertexes();
	return std::make_unique<ve::VulkanModel>(device, vertexes, VOXEL_VERTEX_INDEXES);
}

bool WorldGenerator::addeNewWorld( vec2i const& worldPos ) {
	if (this->worlds.find(worldPos) != this->worlds.end())
		return false;

	vec3 relativeOrigin = this->localPosFromWorldPos(worldPos);
	this->worlds.emplace(worldPos, World(this->worldSize, relativeOrigin));
	this->totVoxels += this->worlds[worldPos].getVertexSize() / VERTEX_PER_VOXEL;

	if (this->worlds.size() > MAX_WORLDS) {
		vec2i furthestWorld = this->findFurthestWorld();
		this->totVoxels -= this->worlds[furthestWorld].getVertexSize() / VERTEX_PER_VOXEL;
		this->worlds.erase(furthestWorld);
	}
	return true;
}

vec2i WorldGenerator::findFurthestWorld( void ) noexcept {
	vec2i furthestWorld = this->currentWorldPos;
	uint32_t furthestDist = 0U, dist = 0U;
	for (auto& [pos, _] : this->worlds) {
		dist = std::abs(pos.x - this->currentWorldPos.x) + std::abs(pos.y - this->currentWorldPos.y);
		if (dist > furthestDist) {
			furthestWorld = pos;
			furthestDist = dist;
		}
	}
	return furthestWorld;
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
