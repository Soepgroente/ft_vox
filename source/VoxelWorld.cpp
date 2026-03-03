#include "VoxelWorld.hpp"
#include "Vectors.hpp"
#include "Config.hpp"

#include <map>


namespace vox {


// floor on the ground, two 'towers' of voxels, left and right
VoxelGrid VoxelGrid::voxelGenerator1( vec3ui const& worldLimit ) {
	VoxelGrid	grid(worldLimit);
	vec3ui		index(0U);
	// set floor
	for(index.y=0; index.y<worldLimit.y; index.y++) {
		for(index.x=0; index.x<worldLimit.x; index.x++)
			grid[index] = true;
	}
	// left tower, 4 voxel base
	for(index.z=1; index.z<worldLimit.z; index.z++) {
		grid[vec3ui{1, worldLimit.y - 2, index.z}] = true;
		grid[vec3ui{2, worldLimit.y - 2, index.z}] = true;
		grid[vec3ui{1, worldLimit.y - 1, index.z}] = true;
		grid[vec3ui{2, worldLimit.y - 1, index.z}] = true;
	}
	// right tower, 4 voxel base
	for(index.z=1; index.z<worldLimit.z; index.z++) {
		grid[vec3ui{5, worldLimit.y - 2, index.z}] = true;
		grid[vec3ui{6, worldLimit.y - 2, index.z}] = true;
		grid[vec3ui{5, worldLimit.y - 1, index.z}] = true;
		grid[vec3ui{6, worldLimit.y - 1, index.z}] = true;
	}

	return grid;
}

// floor on the ground, rest is random
VoxelGrid VoxelGrid::voxelGenerator2( vec3ui const& worldLimit ) {
	VoxelGrid	grid(worldLimit);
	vec3ui		index(0U);

	// set floor
	for(index.y=0; index.y<worldLimit.y; index.y++) {
		for(index.x=0; index.x<worldLimit.x; index.x++)
			grid[index] = true;
	}
	index.x = 0; index.y = 0;
	for(index.z=1; index.z<worldLimit.z; index.z++) {
		for(index.y=0; index.y<worldLimit.y; index.y++) {
			for(index.x=0; index.x<worldLimit.x; index.x++)
				grid[index] = (ve::randomFloat() > 0.85f) ? true : false;
		}
	}

	return grid;
}

// basic random generation
VoxelGrid VoxelGrid::voxelGenerator3( vec3ui const& worldLimit ) {
	VoxelGrid	grid(worldLimit);
	vec3ui		index(0U);

	// set floor
	for(index.y=0; index.y<worldLimit.y; index.y++) {
		for(index.x=0; index.x<worldLimit.x; index.x++)
			grid[index] = true;
	}
	// set ceiling
	for(index.y=0; index.y<worldLimit.y; index.y++) {
		for(index.x=0; index.x<worldLimit.x; index.x++)
			grid[vec3ui{index.x, index.y, worldLimit.z - 1}] = true;
	}
	// four columns on corner
	for(index.z=0; index.z<worldLimit.z; index.z++) {
		grid[vec3ui{0, 0, index.z}] = true;
		grid[vec3ui{worldLimit.x - 1, 0, index.z}] = true;
		grid[vec3ui{0, worldLimit.y - 1, index.z}] = true;
		grid[vec3ui{worldLimit.x - 1, worldLimit.y - 1, index.z}] = true;
	}
	// random cubes, with penality along z (the higher the more difficult the spawn)
	for(index.z=1; index.z<worldLimit.z - 1; index.z++) {
		float t = static_cast<float>(index.z) / worldLimit.z;
		float factor = 1.0f - 1.0f * t;
		for(index.y=1; index.y<worldLimit.y-1; index.y++) {
			for(index.x=1; index.x<worldLimit.x-1; index.x++)
				grid[index] = ((ve::randomFloat() * factor) > 0.3f) ? true : false;
		}
	}

	return grid;
}

// four voxels in the middle
VoxelGrid VoxelGrid::voxelGenerator4( vec3ui const& worldLimit ) {
	VoxelGrid grid(worldLimit);

	grid[vec3ui{(worldLimit.x - 1) / 2, (worldLimit.y - 1) / 2, (worldLimit.z - 1) / 2}] = true;
	grid[vec3ui{(worldLimit.x - 1) / 2 + 1, (worldLimit.y - 1) / 2, (worldLimit.z - 1) / 2}] = true;
	grid[vec3ui{(worldLimit.x - 1) / 2, (worldLimit.y - 1) / 2, (worldLimit.z - 1) / 2 + 1}] = true;
	grid[vec3ui{(worldLimit.x - 1) / 2 + 1, (worldLimit.y - 1) / 2, (worldLimit.z - 1) / 2 + 1}] = true;
	return grid;
}

// sequence of rectangluar prisms, smaller in area going to the top
VoxelGrid VoxelGrid::voxelGenerator5( vec3ui const& worldLimit ) {
	VoxelGrid	grid(worldLimit);
	vec3ui		index(0U);
	uint32_t	hBlock = 0U;

	while (index.z < worldLimit.z) {
		for(; index.z<hBlock+2; index.z++) {
			for(index.y=hBlock; index.y<worldLimit.y - hBlock; index.y++) {
				for(index.x=hBlock; index.x<worldLimit.x - hBlock; index.x++)
					grid[index] = true;
			}
		}
		hBlock += 2;
	}
	return grid;
}

// 4 towers at the corners with different height 
VoxelGrid VoxelGrid::voxelGenerator6( vec3ui const& worldLimit ) {
	VoxelGrid	grid(worldLimit);
	vec3ui		index(0U);
	// set floor
	for(index.y=0; index.y<worldLimit.y; index.y++) {
		for(index.x=0; index.x<worldLimit.x; index.x++)
			grid[index] = true;
	}
	grid[vec3ui{0, 0, 1}] = true;
	grid[vec3ui{0, 0, 2}] = true;
	grid[vec3ui{0, 0, 3}] = true;

	grid[vec3ui{worldLimit.x - 1, 0, 1}] = true;
	grid[vec3ui{worldLimit.x - 1, 0, 2}] = true;
	grid[vec3ui{worldLimit.x - 1, 0, 3}] = true;
	grid[vec3ui{worldLimit.x - 1, 0, 4}] = true;

	grid[vec3ui{0, worldLimit.y - 1, 1}] = true;
	grid[vec3ui{0, worldLimit.y - 1, 2}] = true;
	grid[vec3ui{0, worldLimit.y - 1, 3}] = true;
	grid[vec3ui{0, worldLimit.y - 1, 4}] = true;
	grid[vec3ui{0, worldLimit.y - 1, 5}] = true;

	grid[vec3ui{worldLimit.x - 1, worldLimit.y - 1, 1}] = true;
	grid[vec3ui{worldLimit.x - 1, worldLimit.y - 1, 2}] = true;
	grid[vec3ui{worldLimit.x - 1, worldLimit.y - 1, 3}] = true;
	grid[vec3ui{worldLimit.x - 1, worldLimit.y - 1, 4}] = true;
	grid[vec3ui{worldLimit.x - 1, worldLimit.y - 1, 5}] = true;
	grid[vec3ui{worldLimit.x - 1, worldLimit.y - 1, 6}] = true;
	return grid;
}

// empty box
VoxelGrid VoxelGrid::voxelGenerator7( vec3ui const& worldLimit ) {
	VoxelGrid	grid(worldLimit);
	vec3ui		index(0U);

	// set floor
	for(index.y=0; index.y<worldLimit.y; index.y++) {
		for(index.x=0; index.x<worldLimit.x; index.x++) {
			if (index.y % 2 and index.x % 2)
				continue;
			grid[index] = true;
		}
	}
	// set ceiling
	index.z = worldLimit.z - 1;
	for(index.y=0; index.y<worldLimit.y; index.y++) {
		for(index.x=0; index.x<worldLimit.x; index.x++) {
			if (index.y % 2 or index.x % 2)
				continue;
			grid[index] = true;
		}
	}
	// set left face
	index.x = 0;
	for(index.z=0; index.z<worldLimit.z; index.z++) {
		for(index.y=0; index.y<worldLimit.y; index.y++)
		grid[index] = true;
	}
	// set right face
	index.x = worldLimit.x - 1;
	for(index.z=0; index.z<worldLimit.z; index.z++) {
		for(index.y=0; index.y<worldLimit.y; index.y++)
		grid[index] = true;
	}
	return grid;
}

// floor on the ground
VoxelGrid VoxelGrid::voxelGenerator8( vec3ui const& worldLimit ) {
	VoxelGrid	grid(worldLimit);
	vec3ui		index{0U, 0U, 0U};
	// set floor
	for(index.y=0; index.y<worldLimit.y; index.y++) {
		for(index.x=0; index.x<worldLimit.x; index.x++)
			grid[index] = true;
	}
	return grid;
}

VoxelGrid::VoxelGrid( vec3ui const& size ) {
	this->_size = size;
	this->_grid = std::vector<bool>(this->_size.x * this->_size.y * this->_size.z, false);
}

std::vector<bool>::reference VoxelGrid::operator[]( vec3ui const& pos ) {
	if ((pos.x >= this->_size.x) or
		(pos.y >= this->_size.y) or
		(pos.z >= this->_size.z))
			throw std::runtime_error("Voxel position out of grid");

	return this->_grid[pos.x + pos.y * this->_size.x + pos.z * this->_size.x * this->_size.y];
}

std::vector<bool>::const_reference VoxelGrid::operator[]( vec3ui const& pos ) const {
	if ((pos.x >= this->_size.x) or
		(pos.y >= this->_size.y) or
		(pos.z >= this->_size.z))
			throw std::runtime_error("Voxel position out of grid");

	return this->_grid[pos.x + pos.y * this->_size.x + pos.z * this->_size.x * this->_size.y];
}

vec3ui const& VoxelGrid::getSize( void ) const noexcept {
	return this->_size;
}

bool VoxelGrid::isVoxel( vec3ui const& pos ) const {
	return (*this)[pos];
}

void VoxelGrid::setVoxel( vec3ui const& pos, bool value ) {
	(*this)[pos] = value;
}

void VoxelGrid::setVoxel( vec3ui const& start, vec3ui const& end, bool value ) {
	if ((start.x >= this->_size.x) or
		(start.y >= this->_size.y) or
		(start.z >= this->_size.z) or
		(end.x > this->_size.x) or
		(end.y > this->_size.y) or
		(end.z > this->_size.z))
			throw std::runtime_error("Voxel position(s) out of world");

	vec3ui index = start;
	for (; index.z < end.z; index.z++) {
		for (index.y=start.y; index.y < end.y; index.y++) {
			for (index.x=start.x; index.x < end.x; index.x++)
				(*this)[index] = value;
		}
	}

}

vec3ui VoxelGrid::nextVoxel( vec3ui const& pos ) const {
	if ((pos.x >= this->_size.x) or
		(pos.y >= this->_size.y) or
		(pos.z >= this->_size.z))
			throw std::runtime_error("Voxel position out of grid");

	vec3ui	next(pos);
	do {
		if (next.x < this->_size.x - 1)		// next voxel in line
			next.x++;
		else if (next.y < this->_size.y - 1) {	// end of line, check y+1
			next.x = 0;
			next.y++;
		}
		else if (next.z < this->_size.z - 1) {	 // end of surface, check z+1
			next.x = 0;
			next.y = 0;
			next.z++;
		}
		else										// current voxel is the last one, return {0U, 0U, 0U}
			return vec3ui{0U, 0U, 0U};
	} while (this->isVoxel(next) == false);
	return next;
}

vec3ui VoxelGrid::firstVoxel( void ) const {
	vec3ui	next(0U);

	while (this->isVoxel(next) == false) {
		if (next.x < this->_size.x - 1)		// next voxel in line
			next.x++;
		else if (next.y < this->_size.y - 1) {	// end of line, check y+1
			next.x = 0;
			next.y++;
		}
		else if (next.z < this->_size.z - 1) {	 // end of surface, check z+1
			next.x = 0;
			next.y = 0;
			next.z++;
		}
		else
			throw std::runtime_error("No voxel found in grid");
	}
	return next;
}

std::array<vec3,VERTEX_PER_VOXEL> getVertexRelative( vec3 const& relativeOrigin, vec3ui const& dimension ) {
	std::array<vec3,VERTEX_PER_VOXEL> voxelVertexes;
	for (uint32_t i=0; i<VERTEX_PER_VOXEL; i++) {
		voxelVertexes[i].x = (VOXEL_VERTEXES[i].x + 0.5f) * dimension.x + relativeOrigin.x;
		voxelVertexes[i].y = (VOXEL_VERTEXES[i].y + 0.5f) * dimension.y + relativeOrigin.y;
		voxelVertexes[i].z = (VOXEL_VERTEXES[i].z + 0.5f) * dimension.z + relativeOrigin.z;
	}
	return voxelVertexes;
}


void WorldGenerator::initWorld( void ) {
	this->_builder.emptyData();
	this->addeNewGrid(vec2i(0));
}

bool WorldGenerator::checkSurroundings( vec3 const& playerPos ) {
	vec2i currentWorldPos{
		static_cast<int32_t>(playerPos.x) / static_cast<int32_t>(this->_gridSize.x),
		static_cast<int32_t>(playerPos.y) / static_cast<int32_t>(this->_gridSize.y)
	};
	if (playerPos.x < 0.0f)
		currentWorldPos.x -= 1;
	if (playerPos.y < 0.0f)
		currentWorldPos.y -= 1;

	// add a grid in every of these 9 qudrants
	//  __ __ __
	// |NW|N |NE|
	// |__|__|__|
	// | W| M| E|
	// |__|__|__|
	// |SW|S |SE|
	// |__|__|__|
	bool realoadData = false;
	realoadData |= this->addeNewGrid(vec2i{currentWorldPos.x, currentWorldPos.y + 1});		// N
	realoadData |= this->addeNewGrid(vec2i{currentWorldPos.x + 1, currentWorldPos.y + 1});	// N-E
	realoadData |= this->addeNewGrid(vec2i{currentWorldPos.x + 1, currentWorldPos.y});		// E
	realoadData |= this->addeNewGrid(vec2i{currentWorldPos.x + 1, currentWorldPos.y - 1});	// S-E
	realoadData |= this->addeNewGrid(vec2i{currentWorldPos.x, currentWorldPos.y - 1});		// S
	realoadData |= this->addeNewGrid(vec2i{currentWorldPos.x - 1, currentWorldPos.y - 1});	// S-W
	realoadData |= this->addeNewGrid(vec2i{currentWorldPos.x - 1, currentWorldPos.y});		// W
	realoadData |= this->addeNewGrid(vec2i{currentWorldPos.x - 1, currentWorldPos.y + 1});	// N-W
	return realoadData;
}

bool WorldGenerator::addeNewGrid( vec2i const& gridPos ) {
	bool insertedNewGrid = this->_world.count(gridPos) == 0;

	if (insertedNewGrid) {
		VoxelGrid newGrid = VoxelGrid::voxelGenerator8(this->_gridSize);
		this->_world.try_emplace(gridPos, newGrid);
		this->fillBufferGrid(gridPos);
	}
	return insertedNewGrid;
}

void WorldGenerator::fillBufferGrid( vec2i const centerGrid ) {
	vec3 relativeOrigin{
		static_cast<float>(centerGrid.x * static_cast<int32_t>(this->_gridSize.x)),
		static_cast<float>(centerGrid.y * static_cast<int32_t>(this->_gridSize.y)),
		0.0f
	};
	vec3ui	index(0U);
	for(; index.z<this->_gridSize.z; index.z++) {
		for(index.y=0; index.y<this->_gridSize.y; index.y++) {
			for(index.x=0; index.x<this->_gridSize.x; index.x++) {
				if (this->_world.at(centerGrid).isVoxel(index) == false)
					continue;

				vec3 centerVoxel{
					static_cast<float>(index.x) + relativeOrigin.x,
					static_cast<float>(index.y) + relativeOrigin.y,
					static_cast<float>(index.z)
				};
				std::array<vec3,VERTEX_PER_VOXEL> voxelVertexes = getVertexRelative(centerVoxel);
				// check every vertex of the cube/voxel to avoid duplicates
				for (uint32_t index : VOXEL_VERTEX_INDEXES)
					this->_builder.addVertex(voxelVertexes[index]);
			}
		}
	}
}

void WorldGenerator::fillBufferGridGreedy( vec2i const centerGrid ) {
	vec3 relativeOrigin{
		static_cast<float>(centerGrid.x) * static_cast<float>(this->_gridSize.x),
		static_cast<float>(centerGrid.y) * static_cast<float>(this->_gridSize.y),
	};
	vec3ui			start = this->_world.at(centerGrid).firstVoxel(), curr = start, boxelSize(1U);
	vec3ui const	endingVoxel(0U), wordlLimit = this->_world.at(centerGrid).getSize();
	
	do {	// start == {0,0,0} -> no voxels remain in the grid
		curr = start;
		boxelSize.x = 1U, boxelSize.y = 1U, boxelSize.z = 1U;
		// find longest line of consecutive voxels
		for (curr.x = start.x + 1; curr.x < wordlLimit.x; curr.x++) {
			if (this->_world.at(centerGrid).isVoxel(curr) == false)
				break;
			boxelSize.x++;
		}
		// find widest rectangle of voxels
		for (curr.y = start.y + 1; curr.y < wordlLimit.y; curr.y++) {
			for (curr.x = start.x; curr.x < start.x + boxelSize.x; curr.x++) {
				if (this->_world.at(centerGrid).isVoxel(curr) == false)
					break;
			}
			if (curr.x < start.x + boxelSize.x) break;
			boxelSize.y++;
		}
		// find biggest rectangular prism of voxels
		for (curr.z = start.z + 1; curr.z < wordlLimit.z; curr.z++) {
			for (curr.y = start.y; curr.y < start.y + boxelSize.y; curr.y++) {
				for (curr.x = start.x; curr.x < start.x + boxelSize.x; curr.x++) {
					if (this->_world.at(centerGrid).isVoxel(curr) == false) break;
				}
				if (curr.x < start.x + boxelSize.x) break;
			}
			if ((curr.x < start.x + boxelSize.x) or (curr.y < start.y + boxelSize.y)) break;
			boxelSize.z++;
		}
		// deactivate all the valid past voxels
		// std::cout << "found boxel in: " << start << std::endl;
		this->_world.at(centerGrid).setVoxel(start, start + boxelSize, false);
		// add the newly found boxel
		vec3 centerBoxel{
			static_cast<float>(start.x) + relativeOrigin.x,
			static_cast<float>(start.y) + relativeOrigin.y,
			static_cast<float>(start.z)
		};
		std::array<vec3,VERTEX_PER_VOXEL> voxelVertexes = getVertexRelative(centerBoxel, boxelSize);
		// check every vertex of the cube/voxel to avoid duplicates
		for (uint32_t index : VOXEL_VERTEX_INDEXES)
			this->_builder.addVertex(voxelVertexes[index]);
		// find next voxel
		start = this->_world.at(centerGrid).nextVoxel(start);
	} while (start != endingVoxel);
}

}
