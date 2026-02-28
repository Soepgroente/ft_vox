#include "VoxelWorld.hpp"
#include "Vectors.hpp"

#include <map>


namespace vox {


vec3 Boxel::getCenter( void ) const noexcept {
	return vec3{ 
		static_cast<float>(this->_center.x) + (this->_size.x / 2.0f),
		static_cast<float>(this->_center.y) + (this->_size.y / 2.0f),
		static_cast<float>(this->_center.z) + (this->_size.z / 2.0f)
	};
}

vec3 Boxel::getSize( void ) const noexcept {
	return vec3{
		static_cast<float>(this->_size.x),
		static_cast<float>(this->_size.y),
		static_cast<float>(this->_size.z)
	};
}

std::vector<vec3> Boxel::getVertexes( vec3 const& relativeOrigin ) const noexcept {
	std::vector<vec3>	vertexes(VERTEX_PER_VOXEL);
	vec3				sizeFloat = this->getSize();
	vec3				center = this->getCenter() + vec3{relativeOrigin.x, relativeOrigin.y, 0.0f};

	for (uint32_t i=0; i<VERTEX_PER_VOXEL; i++) {
		// pos = posV * scale-size + center-pos
		vertexes[i].x = center.x + VOXEL_VERTEXES[i].x * sizeFloat.x / 2.0f;
		vertexes[i].y = center.y + VOXEL_VERTEXES[i].y * sizeFloat.y / 2.0f;
		vertexes[i].z = center.z + VOXEL_VERTEXES[i].z * sizeFloat.z / 2.0f;
	}
	return vertexes;
}


vec3 Voxel::getCenter( void ) const noexcept {
	return vec3{
		static_cast<float>(this->_center.x) + 0.5f,
		static_cast<float>(this->_center.y) + 0.5f,
		static_cast<float>(this->_center.z) + 0.5f
	};
}

std::vector<vec3> Voxel::getVertexes( vec3 const& relativeOrigin ) const noexcept {
	std::vector<vec3>	vertexes(VERTEX_PER_VOXEL);
	vec3				center = this->getCenter() + vec3{relativeOrigin.x, relativeOrigin.y, 0.0f};

	for (uint32_t i=0; i<VERTEX_PER_VOXEL; i++)
		vertexes[i] = center + VOXEL_VERTEXES[i] * 0.5f;
	return vertexes;
}


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

std::vector<Voxel>	VoxelGrid::getVoxels( void ) const {
	std::vector<Voxel>	voxels;
	vec3ui				index(0U);

	for(; index.z<this->_size.z; index.z++) {
		for(index.y=0; index.y<this->_size.y; index.y++) {
			for(index.x=0; index.x<this->_size.x; index.x++) {
				if (this->isVoxel(index))
					voxels.push_back(Voxel(index));
			}
		}
	}
	return voxels;
}

std::vector<Boxel> VoxelGrid::getBoxels( void ) {
	vec3ui				start = this->firstVoxel(), curr(start), boxelSize(1U);
	vec3ui const		endingVoxel(0U), wordlLimit = this->getSize();
	std::vector<Boxel>	boxels;

	do {
		curr = start;
		boxelSize.x = 1U, boxelSize.y = 1U, boxelSize.z = 1U;
		// find longest line of consecutive voxels
		for (curr.x = start.x + 1; curr.x < wordlLimit.x; curr.x++) {
			if (this->isVoxel(curr) == false)
				break;
			boxelSize.x++;
		}
		// find widest rectangle of voxels
		for (curr.y = start.y + 1; curr.y < wordlLimit.y; curr.y++) {
			for (curr.x = start.x; curr.x < start.x + boxelSize.x; curr.x++) {
				if (this->isVoxel(curr) == false)
					break;
			}
			if (curr.x < start.x + boxelSize.x) break;
			boxelSize.y++;
		}
		// find biggest rectangular prism of voxels
		for (curr.z = start.z + 1; curr.z < wordlLimit.z; curr.z++) {
			for (curr.y = start.y; curr.y < start.y + boxelSize.y; curr.y++) {
				for (curr.x = start.x; curr.x < start.x + boxelSize.x; curr.x++) {
					if (this->isVoxel(curr) == false) break;
				}
				if (curr.x < start.x + boxelSize.x) break;
			}
			if ((curr.x < start.x + boxelSize.x) or (curr.y < start.y + boxelSize.y)) break;
			boxelSize.z++;
		}
		// add the newly found boxel
		boxels.push_back(Boxel(start, boxelSize));
		// deactivate all the valid past voxels
		this->setVoxel(start, start + boxelSize, false);
		// find next voxel
		start = this->nextVoxel(start);
	} while (start != endingVoxel); // start == {0,0,0} -> no voxels remain in the grid
	return boxels;
}



void WorldGenerator::initWorld(vec3 const& centerGrid ) {
	vec2ui centerGrid2D{
		static_cast<uint32_t>(centerGrid.x),
		static_cast<uint32_t>(centerGrid.y),
	};
	
	this->_currentWorldPos = centerGrid2D;
	this->_world.try_emplace(centerGrid2D, VoxelGrid::voxelGenerator8(this->_gridSize));
	this->generateBufferData();
}

void WorldGenerator::expandWorld( WorldDirection direction ) {
	vec2ui newWorldCenter = this->_currentWorldPos;
	switch (direction)
	{
		case D_NORTH:
			newWorldCenter.y += this->_gridSize.y;
			break;
		case D_WEST:
			newWorldCenter.x -= this->_gridSize.x;
			break;
		case D_SOUTH:
			newWorldCenter.y -= this->_gridSize.y;
			break;
		case D_EAST:
			newWorldCenter.x += this->_gridSize.x;
			break;
		default:
			break;
	}

	this->_world.try_emplace(newWorldCenter, VoxelGrid::voxelGenerator8(this->_gridSize));
	// in an ideal world I should just add the newly created vertexes, not re-create
	// all of them everty time a new world is created
	this->generateBufferData();
}

void WorldGenerator::generateBufferData( bool duplicateVertex ) {
	uint32_t indexCount = 0U;
	uint32_t _debugNvoxels = 0U;
	uint32_t _debugNtriangles = 0U;
	
	for (auto const& [centerGrid, grid] : this->_world) {
		std::unordered_map<vec3, uint32_t>	uniqueVertexes;
		vec3 origin3D {
			static_cast<float>(centerGrid.x),
			static_cast<float>(centerGrid.y),
			2.0f,
		};
		vec3 relativeOrigin = origin3D - vec3{this->_gridSize.x / 2.0f, this->_gridSize.y / 2.0f, 0.0f};

		for (Voxel const& voxel : grid.getVoxels()) {
			std::vector<vec3> voxelVertexes = voxel.getVertexes(relativeOrigin);
			// check every vertex of the cube/voxel to avoid duplicates
			for (uint32_t index : VOXEL_VERTEX_INDEXES) {
				if (duplicateVertex == true) {
					this->_builder.vertices.push_back(ve::VulkanModel::Vertex{
						voxelVertexes[index],
						ve::generateRandomColor(),		// NB until color is random Vertex type can't be use as a key inside the unord. map
						vec3{0.0f, 0.0f, 0.0f},
						vec2{0.0f, 0.0f}
					});
					_debugNtriangles++;			// debug info
					this->_builder.indices.push_back(indexCount++);
				}
				else if (uniqueVertexes.count(voxelVertexes[index]) > 0)
					// there's already such vertex, add only the vertex index
					this->_builder.indices.push_back(uniqueVertexes[voxelVertexes[index]]);
				else {
					uniqueVertexes[voxelVertexes[index]] = indexCount;
					// new vertex, add it and its vertex index
					this->_builder.vertices.push_back(ve::VulkanModel::Vertex{
						voxelVertexes[index],
						ve::generateRandomColor(),		// NB until color is random Vertex type can't be use as a key inside the unord. map
						vec3{0.0f, 0.0f, 0.0f},
						vec2{0.0f, 0.0f}
					});
					_debugNtriangles++;			// debug info
					this->_builder.indices.push_back(indexCount++);
				}
			}
			_debugNvoxels++;			// debug info
		}
	}

	if (this->_debugMode) {
		std::cout << "\n------" << std::endl;
		std::cout << "VoxWorld generation, debug mode:" << std::endl;
		std::cout << "  voxels generated: " << _debugNvoxels << std::endl;
		if (duplicateVertex == true)
			std::cout << "  [duplicated vertexes are inserted in buffer]" << std::endl;
		else
			std::cout << "  [duplicated vertexes are skipped]" << std::endl;
		std::cout << "  vertexes inserted in buffer: " << _debugNtriangles << std::endl;
		std::cout << "  triangles: " << (indexCount + 1) / 3 << std::endl;
		std::cout << "------\n" << std::endl;
	}
}

void WorldGenerator::crossWorldBorder( WorldDirection border ) {
	vec2ui newWorldCenter = this->_currentWorldPos;
	switch (border)
	{
		case D_NORTH:
			newWorldCenter.y += this->_gridSize.y;
			break;
		case D_WEST:
			newWorldCenter.x -= this->_gridSize.x;
			break;
		case D_SOUTH:
			newWorldCenter.y -= this->_gridSize.y;
			break;
		case D_EAST:
			newWorldCenter.x += this->_gridSize.x;
			break;
		default:
			break;
	}
	if (this->_world.count(newWorldCenter) == 0)
		throw std::runtime_error("Error, world not existing");

	this->_currentWorldPos = newWorldCenter;
}

ve::VulkanModel::Builder WorldGenerator::generateBufferDataGreedy( bool duplicateVertex ) {
	ve::VulkanModel::Builder	builder;
	uint32_t 					indexCount = 0U;

	uint32_t	_debugNboxels = 0U;
	uint32_t	_debugNvoxels = 0U;
	uint32_t	_debugNtriangles = 0U;
	
	for (auto& [centerGrid, grid] : this->_world) {
		std::unordered_map<vec3, uint32_t>	uniqueVertexes;
		vec3 origin3D {
			static_cast<float>(centerGrid.x),
			static_cast<float>(centerGrid.y),
			2.0f,
		};
		vec3 relativeOrigin = origin3D - vec3{this->_gridSize.x / 2.0f, this->_gridSize.y / 2.0f, 0.0f};

		for (Boxel const& boxel : grid.getBoxels()) {
			std::vector<vec3> voxelVertexes = boxel.getVertexes(relativeOrigin);
			// check every vertex of the prism/boxel to avoid duplicates
			for (uint32_t index : VOXEL_VERTEX_INDEXES) {
				if (duplicateVertex == true) {
					builder.vertices.push_back(ve::VulkanModel::Vertex{
						voxelVertexes[index],
						ve::generateRandomColor(),		// NB until color is random Vertex type can't be use as a key inside the unord. map
						vec3{0.0f, 0.0f, 0.0f},
						vec2{0.0f, 0.0f}
					});
					builder.indices.push_back(indexCount++);
					_debugNtriangles++;			// debug info
				}
				else if (uniqueVertexes.count(voxelVertexes[index]) > 0)
					// there's already such vertex, add only the vertex index
					builder.indices.push_back(uniqueVertexes[voxelVertexes[index]]);
				else {
					uniqueVertexes[voxelVertexes[index]] = indexCount;
					// new vertex, add it and its vertex index
					builder.vertices.push_back(ve::VulkanModel::Vertex{
						voxelVertexes[index],
						ve::generateRandomColor(),		// NB until color is random Vertex type can't be use as a key inside the unord. map
						vec3{0.0f, 0.0f, 0.0f},
						vec2{0.0f, 0.0f}
					});
					builder.indices.push_back(indexCount++);
					_debugNtriangles++;			// debug info
				}
			}
			_debugNboxels++;	// debug info
		}
	}

	if (this->_debugMode) {
		std::cout << "\n------" << std::endl;
		std::cout << "VoxWorld generation, debug mode:" << std::endl;
		std::cout << "  voxels generated: " << _debugNvoxels << std::endl;
		std::cout << "  boxels generated: " << _debugNboxels << std::endl;
		if (duplicateVertex == true)
			std::cout << "  [duplicated vertexes are inserted in buffer]" << std::endl;
		else
			std::cout << "  [duplicated vertexes are skipped]" << std::endl;
		std::cout << "  vertexes inserted in buffer: " << _debugNtriangles << std::endl;
		std::cout << "  triangles: " << (indexCount + 1) / 3 << std::endl;
		std::cout << "------\n" << std::endl;
	}
	return builder;
}

}
