#include "VoxelWorld.hpp"
#include "Vectors.hpp"
#include "Config.hpp"

#include <map>
#include <chrono>


namespace vox {

VertexArray getVertexRelativeNoTextures( vec3 const& relativeOrigin, vec3ui const& dimension ) {
	VertexArray voxelVertexes;
	for (uint32_t i=0; i<VERTEX_PER_VOXEL; i++) {
		voxelVertexes[i].pos.x = (VOXEL_VERTEXES_NO_TEXT[i].pos.x + 0.5f) * dimension.x + relativeOrigin.x;
		voxelVertexes[i].pos.y = (VOXEL_VERTEXES_NO_TEXT[i].pos.y + 0.5f) * dimension.y + relativeOrigin.y;
		voxelVertexes[i].pos.z = (VOXEL_VERTEXES_NO_TEXT[i].pos.z + 0.5f) * dimension.z + relativeOrigin.z;
		voxelVertexes[i].normal = VOXEL_VERTEXES[i].normal;
		voxelVertexes[i].color = VOXEL_VERTEXES[i].color;
		voxelVertexes[i].textureUv = VOXEL_VERTEXES[i].textureUv;
	}
	return voxelVertexes;
}

VertexArray getVertexRelativeMonoTextures( vec3 const& relativeOrigin, vec3ui const& dimension ) {
	VertexArray voxelVertexes;
	for (uint32_t i=0; i<VERTEX_PER_VOXEL; i++) {
		voxelVertexes[i].pos.x = (VOXEL_VERTEXES[i].pos.x + 0.5f) * dimension.x + relativeOrigin.x;
		voxelVertexes[i].pos.y = (VOXEL_VERTEXES[i].pos.y + 0.5f) * dimension.y + relativeOrigin.y;
		voxelVertexes[i].pos.z = (VOXEL_VERTEXES[i].pos.z + 0.5f) * dimension.z + relativeOrigin.z;
		voxelVertexes[i].normal = VOXEL_VERTEXES[i].normal;
		voxelVertexes[i].color = VOXEL_VERTEXES[i].color;
		voxelVertexes[i].textureUv = VOXEL_VERTEXES[i].textureUv;
	}
	return voxelVertexes;
}

VertexArray getVertexRelativeAtlasTextures( vec3 const& relativeOrigin, vec3ui const& dimension ) {
	VertexArray voxelVertexes;

	for (uint32_t i=0; i<VERTEX_PER_VOXEL; i++) {
		voxelVertexes[i].pos.x = (VOXEL_VERTEXES[i].pos.x + 0.5f) * dimension.x + relativeOrigin.x;
		voxelVertexes[i].pos.y = (VOXEL_VERTEXES[i].pos.y + 0.5f) * dimension.y + relativeOrigin.y;
		voxelVertexes[i].pos.z = (VOXEL_VERTEXES[i].pos.z + 0.5f) * dimension.z + relativeOrigin.z;
		voxelVertexes[i].normal = VOXEL_VERTEXES[i].normal;
		voxelVertexes[i].color = VOXEL_VERTEXES[i].color;
		
	}
	const float PADDING = 0.002f;
	// setup textures coors:
	// reference: textures/terrain_texture-atlas.jpeg
	const float W = 1.0f / 8.0f;  // width of a tile
	const float H = 1.0f / 6.0f;  // height of a tile
	// (0, 0) is top left
	// depends on the atlas choosen
	// std::array<vec2, 6> faces{
	// 	vec2{ 1 * W, 2 * H },  // FRONT  → column 1, row 2
	// 	vec2{ 1 * W, 0 * H },  // BACK   → column 1, row 0
	// 	vec2{ 0 * W, 1 * H },  // LEFT   → column 0, row 1
	// 	vec2{ 2 * W, 1 * H },  // RIGHT  → column 2, row 1
	// 	vec2{ 1 * W, 1 * H },  // TOP    → column 1, row 1
	// 	vec2{ 3 * W, 1 * H },  // BOTTOM → column 3, row 1
	// };
	// front face
	voxelVertexes[0].textureUv = vec2{W + PADDING, 3 * H - PADDING};
	voxelVertexes[1].textureUv = vec2{2 * W - PADDING, 3 * H - PADDING};
	voxelVertexes[2].textureUv = vec2{2 * W - PADDING, 2 * H + PADDING};
	voxelVertexes[3].textureUv = vec2{W + PADDING, 2 * H + PADDING};
	// back face
	voxelVertexes[4].textureUv = vec2{2 * W - PADDING, PADDING};
	voxelVertexes[4 + 1].textureUv = vec2{W + PADDING, PADDING};
	voxelVertexes[4 + 2].textureUv = vec2{W + PADDING, H - PADDING};
	voxelVertexes[4 + 3].textureUv = vec2{2 * W - PADDING, H - PADDING};
	// left face
	voxelVertexes[8].textureUv = vec2{PADDING, H + PADDING};
	voxelVertexes[8 + 1].textureUv = vec2{PADDING, 2 * H - PADDING};
	voxelVertexes[8 + 2].textureUv = vec2{W - PADDING, 2 * H - PADDING};
	voxelVertexes[8 + 3].textureUv = vec2{W - PADDING, H + PADDING};
	// right face
	voxelVertexes[12].textureUv = vec2{3 * W - PADDING, 2 * H - PADDING};
	voxelVertexes[12 + 1].textureUv = vec2{3 * W - PADDING, H + PADDING};
	voxelVertexes[12 + 2].textureUv = vec2{2 * W + PADDING, H + PADDING};
	voxelVertexes[12 + 3].textureUv = vec2{2 * W + PADDING, 2 * H - PADDING};
	// top face
	voxelVertexes[16].textureUv = vec2{W + PADDING, 2 * H - PADDING};
	voxelVertexes[16 + 1].textureUv = vec2{2 * W - PADDING, 2 * H - PADDING};
	voxelVertexes[16 + 2].textureUv = vec2{2 * W - PADDING, H + PADDING};
	voxelVertexes[16 + 3].textureUv = vec2{W + PADDING, H + PADDING};
	// back face
	voxelVertexes[20].textureUv = vec2{3 * W + PADDING, H + PADDING};
	voxelVertexes[20 + 1].textureUv = vec2{3 * W + PADDING, 2 * H - PADDING};
	voxelVertexes[20 + 2].textureUv = vec2{4 * W - PADDING, 2 * H - PADDING};
	voxelVertexes[20 + 3].textureUv = vec2{4 * W - PADDING, H + PADDING};
	
	return voxelVertexes;
}

std::array<uint32_t, INDEX_PER_VOXEL> getIndexRelative( uint32_t start ) {
	std::array<uint32_t, INDEX_PER_VOXEL> indexes;
	for (uint32_t i=0; i<INDEX_PER_VOXEL; i++)
		indexes[i] = VOXEL_VERTEX_INDEXES[i] + start;
	return indexes;
}

VoxelWorld::WorldIterator& VoxelWorld::WorldIterator::operator++( void ) {
	this->pos3D.x++;
	if (this->pos3D.x == this->limits.x) {
		this->pos3D.y++;
		if (this->pos3D.y == this->limits.y) {
			this->pos3D.z++;
			if (this->pos3D.z < this->limits.z) {
				this->pos3D.x = 0U;
				this->pos3D.y = 0U;
			}	// else pos3D = limits == end
		} else
			this->pos3D.x = 0U;
	}
	return *this;
}

bool VoxelWorld::WorldIterator::operator!=( VoxelWorld::WorldIterator const& other ) const {
	return this->pos3D != other.pos3D;
}


// floor on the ground, two 'towers' of voxels, left and right
VoxelWorld VoxelWorld::voxelGenerator1( vec3ui const& worldLimit ) {
	VoxelWorld	newWorld(worldLimit);
	vec3ui		index(0U);
	// set floor
	for(index.y=0; index.y<worldLimit.y; index.y++) {
		for(index.x=0; index.x<worldLimit.x; index.x++)
			newWorld[index] = true;
	}
	// left tower, 4 voxel base
	for(index.z=1; index.z<worldLimit.z; index.z++) {
		newWorld[vec3ui{1, worldLimit.y - 2, index.z}] = true;
		newWorld[vec3ui{2, worldLimit.y - 2, index.z}] = true;
		newWorld[vec3ui{1, worldLimit.y - 1, index.z}] = true;
		newWorld[vec3ui{2, worldLimit.y - 1, index.z}] = true;
	}
	// right tower, 4 voxel base
	for(index.z=1; index.z<worldLimit.z; index.z++) {
		newWorld[vec3ui{5, worldLimit.y - 2, index.z}] = true;
		newWorld[vec3ui{6, worldLimit.y - 2, index.z}] = true;
		newWorld[vec3ui{5, worldLimit.y - 1, index.z}] = true;
		newWorld[vec3ui{6, worldLimit.y - 1, index.z}] = true;
	}

	return newWorld;
}

// floor on the ground, rest is random
VoxelWorld VoxelWorld::voxelGenerator2( vec3ui const& worldLimit ) {
	VoxelWorld	newWorld(worldLimit);
	vec3ui		index(0U);

	// set floor
	for(index.y=0; index.y<worldLimit.y; index.y++) {
		for(index.x=0; index.x<worldLimit.x; index.x++)
			newWorld[index] = true;
	}
	index.x = 0; index.y = 0;
	for(index.z=1; index.z<worldLimit.z; index.z++) {
		for(index.y=0; index.y<worldLimit.y; index.y++) {
			for(index.x=0; index.x<worldLimit.x; index.x++)
				newWorld[index] = (ve::randomFloat() > 0.85f) ? true : false;
		}
	}

	return newWorld;
}

// basic random generation
VoxelWorld VoxelWorld::voxelGenerator3( vec3ui const& worldLimit ) {
	VoxelWorld	newWorld(worldLimit);
	vec3ui		index(0U);

	// set floor
	for(index.y=0; index.y<worldLimit.y; index.y++) {
		for(index.x=0; index.x<worldLimit.x; index.x++)
			newWorld[index] = true;
	}
	// set ceiling
	for(index.y=0; index.y<worldLimit.y; index.y++) {
		for(index.x=0; index.x<worldLimit.x; index.x++)
			newWorld[vec3ui{index.x, index.y, worldLimit.z - 1}] = true;
	}
	// four columns on corner
	for(index.z=0; index.z<worldLimit.z; index.z++) {
		newWorld[vec3ui{0, 0, index.z}] = true;
		newWorld[vec3ui{worldLimit.x - 1, 0, index.z}] = true;
		newWorld[vec3ui{0, worldLimit.y - 1, index.z}] = true;
		newWorld[vec3ui{worldLimit.x - 1, worldLimit.y - 1, index.z}] = true;
	}
	// random cubes, with penality along z (the higher the more difficult the spawn)
	for(index.z=1; index.z<worldLimit.z - 1; index.z++) {
		float t = static_cast<float>(index.z) / worldLimit.z;
		float factor = 1.0f - 1.0f * t;
		for(index.y=1; index.y<worldLimit.y-1; index.y++) {
			for(index.x=1; index.x<worldLimit.x-1; index.x++)
				newWorld[index] = ((ve::randomFloat() * factor) > 0.3f) ? true : false;
		}
	}

	return newWorld;
}

// four voxels in the middle
VoxelWorld VoxelWorld::voxelGenerator4( vec3ui const& worldLimit ) {
	VoxelWorld newWorld(worldLimit);

	newWorld[vec3ui{(worldLimit.x - 1) / 2, (worldLimit.y - 1) / 2, (worldLimit.z - 1) / 2}] = true;
	newWorld[vec3ui{(worldLimit.x - 1) / 2 + 1, (worldLimit.y - 1) / 2, (worldLimit.z - 1) / 2}] = true;
	newWorld[vec3ui{(worldLimit.x - 1) / 2, (worldLimit.y - 1) / 2, (worldLimit.z - 1) / 2 + 1}] = true;
	newWorld[vec3ui{(worldLimit.x - 1) / 2 + 1, (worldLimit.y - 1) / 2, (worldLimit.z - 1) / 2 + 1}] = true;
	return newWorld;
}

// sequence of rectangluar prisms, smaller in area going to the top
VoxelWorld VoxelWorld::voxelGenerator5( vec3ui const& worldLimit ) {
	VoxelWorld	newWorld(worldLimit);
	vec3ui		index(0U);
	uint32_t	hBlock = 0U;

	while (index.z < worldLimit.z) {
		for(; index.z<hBlock+2; index.z++) {
			for(index.y=hBlock; index.y<worldLimit.y - hBlock; index.y++) {
				for(index.x=hBlock; index.x<worldLimit.x - hBlock; index.x++)
					newWorld[index] = true;
			}
		}
		hBlock += 2;
	}
	return newWorld;
}

// 4 towers at the corners with different height 
VoxelWorld VoxelWorld::voxelGenerator6( vec3ui const& worldLimit ) {
	VoxelWorld	newWorld(worldLimit);
	vec3ui		index(0U);
	// set floor
	for(index.y=0; index.y<worldLimit.y; index.y++) {
		for(index.x=0; index.x<worldLimit.x; index.x++)
			newWorld[index] = true;
	}
	newWorld[vec3ui{0, 0, 1}] = true;
	newWorld[vec3ui{0, 0, 2}] = true;
	newWorld[vec3ui{0, 0, 3}] = true;

	newWorld[vec3ui{worldLimit.x - 1, 0, 1}] = true;
	newWorld[vec3ui{worldLimit.x - 1, 0, 2}] = true;
	newWorld[vec3ui{worldLimit.x - 1, 0, 3}] = true;
	newWorld[vec3ui{worldLimit.x - 1, 0, 4}] = true;

	newWorld[vec3ui{0, worldLimit.y - 1, 1}] = true;
	newWorld[vec3ui{0, worldLimit.y - 1, 2}] = true;
	newWorld[vec3ui{0, worldLimit.y - 1, 3}] = true;
	newWorld[vec3ui{0, worldLimit.y - 1, 4}] = true;
	newWorld[vec3ui{0, worldLimit.y - 1, 5}] = true;

	newWorld[vec3ui{worldLimit.x - 1, worldLimit.y - 1, 1}] = true;
	newWorld[vec3ui{worldLimit.x - 1, worldLimit.y - 1, 2}] = true;
	newWorld[vec3ui{worldLimit.x - 1, worldLimit.y - 1, 3}] = true;
	newWorld[vec3ui{worldLimit.x - 1, worldLimit.y - 1, 4}] = true;
	newWorld[vec3ui{worldLimit.x - 1, worldLimit.y - 1, 5}] = true;
	newWorld[vec3ui{worldLimit.x - 1, worldLimit.y - 1, 6}] = true;
	return newWorld;
}

// empty box
VoxelWorld VoxelWorld::voxelGenerator7( vec3ui const& worldLimit ) {
	VoxelWorld	newWorld(worldLimit);
	vec3ui		index(0U);

	// set floor
	for(index.y=0; index.y<worldLimit.y; index.y++) {
		for(index.x=0; index.x<worldLimit.x; index.x++) {
			if (index.y % 2 and index.x % 2)
				continue;
			newWorld[index] = true;
		}
	}
	// set ceiling
	index.z = worldLimit.z - 1;
	for(index.y=0; index.y<worldLimit.y; index.y++) {
		for(index.x=0; index.x<worldLimit.x; index.x++) {
			if (index.y % 2 or index.x % 2)
				continue;
			newWorld[index] = true;
		}
	}
	// set left face
	index.x = 0;
	for(index.z=0; index.z<worldLimit.z; index.z++) {
		for(index.y=0; index.y<worldLimit.y; index.y++)
		newWorld[index] = true;
	}
	// set right face
	index.x = worldLimit.x - 1;
	for(index.z=0; index.z<worldLimit.z; index.z++) {
		for(index.y=0; index.y<worldLimit.y; index.y++)
		newWorld[index] = true;
	}
	return newWorld;
}

// floor on the ground
VoxelWorld VoxelWorld::voxelGenerator8( vec3ui const& worldLimit ) {
	VoxelWorld	newWorld(worldLimit);
	vec3ui		index{0U, 0U, 0U};
	// set floor
	for(index.y=0; index.y<worldLimit.y; index.y++) {
		for(index.x=0; index.x<worldLimit.x; index.x++)
			newWorld[index] = true;
	}
	return newWorld;
}

VoxelWorld::VoxelWorld( vec3ui const& size ) {
	this->size = size;
	this->world = std::vector<bool>(this->size.x * this->size.y * this->size.z, false);
}

std::vector<bool>::reference VoxelWorld::operator[]( vec3ui const& pos ) {
	if ((pos.x >= this->size.x) or
		(pos.y >= this->size.y) or
		(pos.z >= this->size.z))
			throw std::runtime_error("Voxel position out of world");

	return this->world[pos.x + pos.y * this->size.x + pos.z * this->size.x * this->size.y];
}

std::vector<bool>::const_reference VoxelWorld::operator[]( vec3ui const& pos ) const {
	if ((pos.x >= this->size.x) or
		(pos.y >= this->size.y) or
		(pos.z >= this->size.z))
			throw std::runtime_error("Voxel position out of world");

	return this->world[pos.x + pos.y * this->size.x + pos.z * this->size.x * this->size.y];
}

bool VoxelWorld::isVoxel( vec3ui const& pos ) const {
	return (*this)[pos];
}

void VoxelWorld::setVoxel( vec3ui const& pos, bool value ) {
	(*this)[pos] = value;
}

void VoxelWorld::setVoxel( vec3ui const& start, vec3ui const& end, bool value ) {
	if ((start.x >= this->size.x) or
		(start.y >= this->size.y) or
		(start.z >= this->size.z) or
		(end.x > this->size.x) or
		(end.y > this->size.y) or
		(end.z > this->size.z))
			throw std::runtime_error("Voxel position(s) out of world");

	vec3ui index = start;
	for (; index.z < end.z; index.z++) {
		for (index.y=start.y; index.y < end.y; index.y++) {
			for (index.x=start.x; index.x < end.x; index.x++)
				(*this)[index] = value;
		}
	}

}

vec3ui VoxelWorld::getBoxelSize( vec3ui const& startVoxel ) const noexcept {
	if (this->isVoxel(startVoxel) == false)
		return vec3ui(0U);

	vec3ui start(startVoxel), next(startVoxel), boxelSize(1U);
	// find longest line of consecutive voxels
	for (next.x = start.x + 1; next.x < this->size.x; next.x++) {
		if (this->isVoxel(next) == false)
			break;
		boxelSize.x++;
	}
	// find widest rectangle of voxels
	for (next.y = start.y + 1; next.y < this->size.y; next.y++) {
		for (next.x = start.x; next.x < start.x + boxelSize.x; next.x++) {
			if (this->isVoxel(next) == false)
				break;
		}
		if (next.x < start.x + boxelSize.x) break;
		boxelSize.y++;
	}
	// find biggest rectangular prism of voxels
	for (next.z = start.z + 1; next.z < this->size.z; next.z++) {
		for (next.y = start.y; next.y < start.y + boxelSize.y; next.y++) {
			for (next.x = start.x; next.x < start.x + boxelSize.x; next.x++) {
				if (this->isVoxel(next) == false) break;
			}
			if (next.x < start.x + boxelSize.x) break;
		}
		if ((next.x < start.x + boxelSize.x) or (next.y < start.y + boxelSize.y)) break;
		boxelSize.z++;
	}
	return boxelSize;
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
		// NB it should also remove the world from GPU and reload model
	}
	history.push_back(newPos);
	++counter[newPos];
}

bool WorldGenerator::HistoryWorlds::hasVisited(vec2i const& pos) const {
	return this->counter.find(pos) != this->counter.end();
}

void WorldGenerator::init( void ) {
	this->builder.emptyData();
	this->addeNewWorld(vec2i(0));
}

bool WorldGenerator::spawnCloseByWorlds( vec3 const& playerPos ) {
	vec2i currentWorldPos{
		static_cast<int32_t>(playerPos.x) / static_cast<int32_t>(this->worldSize.x),
		static_cast<int32_t>(playerPos.y) / static_cast<int32_t>(this->worldSize.y)
	};
	if (playerPos.x < 0.0f)
		currentWorldPos.x -= 1;
	if (playerPos.y < 0.0f)
		currentWorldPos.y -= 1;

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
		if (this->mode == MODE_VOXEL_STATIC)
			// every single voxel is loaded directly inside the buffer
			this->fillBufferVoxel2(worldPos);
		else if (this->mode == MODE_BOXEL)
			// creates a temporary world of voxels, and then run greedy meshing algo
			// to create boxels (aggregates with less vertexes), takes 2 ~ 3 ms to spawn the world
			this->fillBufferBoxel(worldPos);
		else if (this->mode == MODE_VOXEL_STATIC) {/* for voxels procedurally generated, TBD*/} 
	}
	return newWorldAdded;
}

void WorldGenerator::fillBufferVoxel1( vec2i const& worldPos ) {
	vec2 relativeOrigin{
		static_cast<float>(worldPos.x * static_cast<int32_t>(this->worldSize.x)),
		static_cast<float>(worldPos.y * static_cast<int32_t>(this->worldSize.y))
	};
	vec3ui		index(0U);
	uint32_t	globalIndexCount = 0U;

	for(; index.z<this->worldSize.z; index.z++) {
		for(index.x=0; index.x<this->worldSize.x; index.x++) {
			vec3 centerVoxel{
				static_cast<float>(index.x) + relativeOrigin.x,
				static_cast<float>(index.y),
				static_cast<float>(index.z) + relativeOrigin.y,
			};
			VertexArray vertexes = getVertexRelativeAtlasTextures(centerVoxel);
			std::array<uint32_t, INDEX_PER_VOXEL> indexes = getIndexRelative(globalIndexCount);

			this->builder.vertices.insert(this->builder.vertices.begin(), vertexes.begin(), vertexes.end());
			this->builder.indices.insert(this->builder.indices.begin(), indexes.begin(), indexes.end());
			globalIndexCount += VERTEX_PER_VOXEL;
		}
	}
}

void WorldGenerator::fillBufferVoxel2( vec2i const& worldPos ) {
	vec3 centerVoxel{
		(this->worldSize.x - 1U) / 2.0f + static_cast<float>(worldPos.x * static_cast<int32_t>(this->worldSize.x)),
		2.0f,
		(this->worldSize.z - 1U) / 2.0f + static_cast<float>(worldPos.y * static_cast<int32_t>(this->worldSize.z)),
	};
	VertexArray vertexes = getVertexRelativeAtlasTextures(centerVoxel);
	std::array<uint32_t, INDEX_PER_VOXEL> indexes = getIndexRelative(0U);

	this->builder.vertices.insert(this->builder.vertices.begin(), vertexes.begin(), vertexes.end());
	this->builder.indices.insert(this->builder.indices.begin(), indexes.begin(), indexes.end());
}

void WorldGenerator::fillBufferBoxel( vec2i const& worldPos ) {
	VoxelWorld newWorld = VoxelWorld::voxelGenerator8(this->worldSize);
	vec2 relativeOrigin{
		static_cast<float>(worldPos.x) * static_cast<float>(this->worldSize.x),
		static_cast<float>(worldPos.y) * static_cast<float>(this->worldSize.y),
	};

	for (auto current3Dpos = newWorld.begin(); current3Dpos != newWorld.end(); ++current3Dpos) {
		if (newWorld.isVoxel(*current3Dpos) == false)
			continue;

		vec3ui start = *current3Dpos;
		// find the size of the boxel starting in current3Dpos
		vec3ui boxelSize = newWorld.getBoxelSize(start);
		// deactivate all the voxels contained in the boxel
		newWorld.setVoxel(start, start + boxelSize, false);
		// add the newly found boxel
		vec3 centerBoxel{
			static_cast<float>(start.x) + relativeOrigin.x,
			static_cast<float>(start.y) + relativeOrigin.y,
			static_cast<float>(start.z)
		};
		VertexArray voxelVertexes = getVertexRelativeNoTextures(centerBoxel, boxelSize);
		// check every vertex of the cube/voxel to avoid duplicates
		for (uint32_t index : VOXEL_VERTEX_INDEXES)
			this->builder.addVertex(voxelVertexes[index]);
	}
}

}
