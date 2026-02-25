#include "VoxelWorld.hpp"
#include "Vectors.hpp"

#include <map>


namespace vox {


vec3 Boxel::getCenter( void ) const noexcept {
	return vec3{
		static_cast<float>(this->_center.x) + Voxel::VOXEL_EDGE * static_cast<float>(this->_size.x) / 2.0f,
		static_cast<float>(this->_center.y) + Voxel::VOXEL_EDGE * static_cast<float>(this->_size.y) / 2.0f,
		static_cast<float>(this->_center.z) + Voxel::VOXEL_EDGE * static_cast<float>(this->_size.z) / 2.0f
	};
}

vec3ui const& Boxel::getSize( void ) const noexcept {
	return this->_size;
}

std::vector<vec3> Boxel::getVertexes( void ) const noexcept {
	std::vector<vec3>	vertexes(VERTEX_PER_VOXEL);
	vec3				centerFloat = this->getCenter();

	for (uint32_t i=0; i<VERTEX_PER_VOXEL; i++) {
		vertexes[i].x = centerFloat.x + VOXEL_VERTEXES[i].x * this->_size.x / 2.0f;
		vertexes[i].y = centerFloat.y + VOXEL_VERTEXES[i].y * this->_size.y / 2.0f;
		vertexes[i].z = centerFloat.z + VOXEL_VERTEXES[i].z * this->_size.z / 2.0f;
	}
	return vertexes;
}


vec3 Voxel::getCenter( void ) const noexcept {
	return vec3{
		static_cast<float>(this->_center.x) + Voxel::VOXEL_EDGE / 2.0f,
		static_cast<float>(this->_center.y) + Voxel::VOXEL_EDGE / 2.0f,
		static_cast<float>(this->_center.z) + Voxel::VOXEL_EDGE / 2.0f
	};
}

float Voxel::getSize( void ) const noexcept {
	return Voxel::VOXEL_EDGE;
}

std::vector<vec3> Voxel::getVertexes( void ) const noexcept {
	vec3 centerFloat = this->getCenter();
	std::vector<vec3> vertexes(VERTEX_PER_VOXEL);

	for (uint32_t i=0; i<VERTEX_PER_VOXEL; i++)
		vertexes[i] = centerFloat + VOXEL_VERTEXES[i] * Voxel::VOXEL_EDGE / 2.0f;
	return vertexes;
}


// floor on the ground, two 'towers' of voxels, left and right
std::vector<bool> voxelGenerator1( uint32_t maxW, uint32_t maxL, uint32_t maxH ) {
	std::vector<bool> grid(maxW * maxL * maxH, false);
	// set floor
	for(uint32_t j=0; j<maxL; j++) {
		for(uint32_t i=0; i<maxW; i++)
			grid[i + j * maxW] = true;
	}
	// left tower, 4 voxel base
	for(uint32_t i=1; i<maxH; i++) {
		grid[1 + (maxW - 2) * maxW + i * maxW * maxL] = true;
		grid[2 + (maxW - 2) * maxW + i * maxW * maxL] = true;
		grid[1 + (maxW - 1) * maxW + i * maxW * maxL] = true;
		grid[2 + (maxW - 1) * maxW + i * maxW * maxL] = true;
	}
	// right tower, 4 voxel base
	for(uint32_t i=1; i<maxH; i++) {
		grid[5 + (maxW - 2) * maxW + i * maxW * maxL] = true;
		grid[6 + (maxW - 2) * maxW + i * maxW * maxL] = true;
		grid[5 + (maxW - 1) * maxW + i * maxW * maxL] = true;
		grid[6 + (maxW - 1) * maxW + i * maxW * maxL] = true;
	}

	return grid;
}

// floor on the ground, rest is random
std::vector<bool> voxelGenerator2( uint32_t maxW, uint32_t maxL, uint32_t maxH ) {
	std::vector<bool> grid(maxW * maxL * maxH, false);
	// set floor
	for(uint32_t j=0; j<maxL; j++) {
		for(uint32_t i=0; i<maxW; i++)
			grid[i + j * maxW] = true;
	}
	for(uint32_t k=1; k<maxH; k++) {
		for(uint32_t j=0; j<maxL; j++) {
			for(uint32_t i=0; i<maxW; i++) {
				bool value = (ve::randomFloat() > 0.85f) ? true : false;
				grid[i + j * maxW + k * maxW * maxL] = value;
			}
		}
	}

	return grid;
}

// basic random generation
std::vector<bool> voxelGenerator3( uint32_t maxW, uint32_t maxL, uint32_t maxH ) {
	std::vector<bool> grid(maxW * maxL * maxH, false);

	// set floor
	for(uint32_t j=0; j<maxL; j++) {
		for(uint32_t i=0; i<maxW; i++)
			grid[i + j * maxW] = true;
	}
	// set ceiling
	for(uint32_t j=0; j<maxL; j++) {
		for(uint32_t i=0; i<maxW; i++)
			grid[i + j * maxW + (maxH - 1) * maxW * maxL] = true;
	}
	// four columns on corner
	for(uint32_t i=0; i<maxH; i++) {
		grid[i * maxL * maxW] = true;
		grid[(maxW - 1) + i * maxL * maxW] = true;
		grid[(maxL - 1) * maxW + i * maxL * maxW] = true;
		grid[(maxW - 1) + (maxL - 1) * maxW + i * maxL * maxW] = true;
	}
	// random cubes, with penality along z (the higher the more difficult the spawn)
	for(uint32_t k=1; k<maxH - 1; k++) {
		float t = static_cast<float>(k) / maxH;
		float factor = 1.0f - 1.0f * t;
		for(uint32_t j=1; j<maxL-1; j++) {
			for(uint32_t i=1; i<maxW-1; i++) {
				bool value = (ve::randomFloat() * factor) > 0.3f;
				grid[i + j * maxW + k * maxW * maxL] = value;
			}
		}
	}

	return grid;
}

// four voxel in the middle
std::vector<bool> voxelGenerator4( uint32_t maxW, uint32_t maxL, uint32_t maxH ) {
	std::vector<bool> grid(maxW * maxL * maxH, false);

	grid[maxW / 2 + (maxL / 2) * maxW + (maxH / 2) * maxW * maxL] = true;
	grid[maxW / 2 + 1 + (maxL / 2) * maxW + (maxH / 2) * maxW * maxL] = true;
	grid[maxW / 2 + (maxL / 2 + 1) * maxW + (maxH / 2) * maxW * maxL] = true;
	grid[maxW / 2 + 1 + (maxL / 2 + 1) * maxW + (maxH / 2) * maxW * maxL] = true;
	return grid;
}

// sequence of rectangluar prisms, smaller in area going to the top
std::vector<bool> voxelGenerator5( uint32_t maxW, uint32_t maxL, uint32_t maxH ) {
	std::vector<bool> grid(maxW * maxL * maxH, false);
	uint32_t h = 0U;

	for(; h<2; h++) {
		for(uint32_t j=0; j<maxL; j++) {
			for(uint32_t i=0; i<maxW; i++)
				grid[i + j * maxW + h * maxW * maxL] = true;
		}
	}
	if (4 < maxH) {
		for(; h<4; h++) {
			for(uint32_t j=2; j<maxL-2; j++) {
				for(uint32_t i=2; i<maxW-2; i++)
					grid[i + j * maxW + h * maxW * maxL] = true;
			}
		}
		if (6 < maxH) {
			for(; h<6; h++) {
				for(uint32_t j=4; j<maxL-4; j++) {
					for(uint32_t i=4; i<maxW-4; i++)
						grid[i + j * maxW + h * maxW * maxL] = true;
				}
			}
			if (8 < maxH) {
				for(; h<8; h++) {
					for(uint32_t j=6; j<maxL-6; j++) {
						for(uint32_t i=6; i<maxW-6; i++)
							grid[i + j * maxW + h * maxW * maxL] = true;
					}
				}
			}
		}
	}

	return grid;
}

// apply textures and normal
//
// std::vector<VectUI3> vertexIndex = face.getIndexes();
// 		std::array<VectF3, 3> triangle{this->_vertexes[vertexIndex[0].i1], this->_vertexes[vertexIndex[1].i1], this->_vertexes[vertexIndex[2].i1]};
//
// 		VectF3 normal = getNormal(triangle);
// 		// texture
// 		VectF3 helper;
// 		if (fabs(normal.x) < 0.9f)
// 			helper = VectF3{1.0f, 0.0f, 0.0f};
// 		else
// 			helper = VectF3{0.0f, 1.0f, 0.0f};
// 		// building a basis orthonormal on the triangle
// 		VectF3 u = normalize(helper ^ normal);
// 		VectF3 v = normalize(normal ^ u);
// 		std::array<float,3> uCoors;
// 		std::array<float,3> vCoors;
// 		// projecting vertexes on the plane
// 		for (uint32_t i=0; i<vertexIndex.size(); i++) {
// 			uCoors[i] = triangle[i] * u;
// 			vCoors[i] = triangle[i] * v;
// 		}
// 		// ranges to normalise the projections in [0, 1]
// 		float uMin = *std::min_element(uCoors.cbegin(), uCoors.cend());
// 		float uMax = *std::max_element(uCoors.cbegin(), uCoors.cend());
// 		float vMin = *std::min_element(vCoors.cbegin(), vCoors.cend());
// 		float vMax = *std::max_element(vCoors.cbegin(), vCoors.cend());
// 		// normal
// 		// to avoid that small faces affect the result too much,
// 		// every normal is weighted with the area of its triangle
// 		float areaTriangle = 0.5f * getAbs(normal);
// 		for (uint32_t i=0; i<vertexIndex.size(); i++) {
// 			VectF2 uv{(uCoors[i] - uMin) / (uMax - uMin), (vCoors[i] - vMin) / (vMax - vMin)};
// 			this->_textures.push_back(uv);
//
// 			VectF3 smoothed = (this->_vertexes[vertexIndex[i].i1] + normal) * areaTriangle;
// 			this->_normals.push_back(normalize(smoothed));
//
// 			vertexIndex[i].i2 = textureIndex++;
// 			vertexIndex[i].i3 = normalIndex++;
// 		}


VoxelWorld::VoxelWorld( vec3ui const& worldSize ) {
	this->_worldSize = worldSize;
}

ve::VulkanModel::Builder VoxelWorld::generateBufferData( std::vector<bool> (&generator)( uint32_t, uint32_t, uint32_t) ) {
	this->_grid = generator(this->_worldSize.x, this->_worldSize.y, this->_worldSize.z);

	ve::VulkanModel::Builder			builder;
	std::unordered_map<vec3, uint32_t>	uniqueVertexes;
	uint32_t 							indexCount = 0U;

	for (uint32_t z=0; z<this->_worldSize.x; z++) {
		for (uint32_t y=0; y<this->_worldSize.y; y++) {
			for (uint32_t x=0; x<this->_worldSize.z; x++) {
				if (this->isVoxel(x, y, z) == false)
					continue;
				// found a voxel, it has 8 vertexes, add the vertex only if it's unique
				Voxel				voxel(vec3ui{x, z, y});		// NB: invertion of y and z
				std::vector<vec3>	voxelVertexes = voxel.getVertexes();
				for (uint32_t index : VOXEL_VERTEX_INDEXES) {
					if (uniqueVertexes.count(voxelVertexes[index]) > 0)
						// there's already such vertex, add only the vertex index
						builder.indices.push_back(uniqueVertexes[voxelVertexes[index]]);
					else {
						uniqueVertexes[voxelVertexes[index]] = indexCount;
						// new vertex, add it and its vertex index
						builder.vertices.push_back(ve::VulkanModel::Vertex{
							voxelVertexes[index],
							ve::generateRandomColor(),
							vec3{0.0f, 0.0f, 0.0f},
							vec2{0.0f, 0.0f}
						});
						builder.indices.push_back(indexCount++);
					}
				}
			}
		}
	}
	return builder;
}

ve::VulkanModel::Builder VoxelWorld::generateBufferDataGreedy( std::vector<bool> (&generator)( uint32_t, uint32_t, uint32_t) ) {
	this->_grid = generator(this->_worldSize.x, this->_worldSize.y, this->_worldSize.z);
	std::vector<Boxel> boxels = this->greedyMeshing();

	ve::VulkanModel::Builder			builder;
	std::unordered_map<vec3, uint32_t>	uniqueVertexes;
	uint32_t 							indexCount = 0U;
	builder.vertices.reserve(boxels.size() * INDEX_PER_VOXEL);
	builder.indices.reserve(boxels.size() * INDEX_PER_VOXEL);

	for (Boxel const& voxel : boxels) {
		std::vector<vec3> voxelVertexes = voxel.getVertexes();
		for (uint32_t index : VOXEL_VERTEX_INDEXES) {
			if (uniqueVertexes.count(voxelVertexes[index]) > 0)
				builder.indices.push_back(uniqueVertexes[voxelVertexes[index]]);
			else {
				uniqueVertexes[voxelVertexes[index]] = indexCount;
				builder.vertices.push_back(ve::VulkanModel::Vertex{
					voxelVertexes[index],
					ve::generateRandomColor(),
					vec3{0.0f, 0.0f, 0.0f},
					vec2{0.0f, 0.0f}
				});
				builder.indices.push_back(indexCount++);
			}
		}
	}
	return builder;
}

std::vector<Boxel> VoxelWorld::greedyMeshing( void ) {
	vec3ui start(0U), curr(0U), boxelSize(1U);
	std::vector<Boxel> boxels;

	while (true) {
		while (this->isVoxel(start) == false) {
			start = this->nextVoxel(start);
			if (start == vec3ui{0U, 0U, 0U})
				return boxels; // no voxel remain in the grid, end of algorithm
		}
		curr = start;
		boxelSize.x = 1U, boxelSize.y = 1U, boxelSize.z = 1U;
		// find longest line of consecutive voxels
		for (curr.x = start.x + 1; curr.x < this->_worldSize.x; curr.x++) {
			if (this->isVoxel(curr) == false)
				break;
			boxelSize.x++;
		}
		// find widest rectangle of voxels
		for (curr.y = start.y + 1; curr.y < this->_worldSize.y; curr.y++) {
			for (curr.x = start.x; curr.x < start.x + boxelSize.x; curr.x++) {
				if (this->isVoxel(curr) == false)
					break;
			}
			if (curr.x < start.x + boxelSize.x) break;
			boxelSize.y++;
		}
		// find biggest rectangular prism of voxels
		for (curr.z = start.z + 1; curr.z < this->_worldSize.z; curr.z++) {
			for (curr.y = start.y; curr.y < start.y + boxelSize.y; curr.y++) {
				for (curr.x = start.x; curr.x < start.x + boxelSize.x; curr.x++) {
					if (this->isVoxel(curr) == false) break;
				}
				if (curr.x < start.x + boxelSize.x) break;
			}
			if ((curr.x < start.x + boxelSize.x) or (curr.y < start.y + boxelSize.y)) break;
			boxelSize.z++;
		}
		// add the newly found boxel, invert Y and Z!
		boxels.push_back(Boxel(vec3ui{start.x, start.z, start.y}, vec3ui{boxelSize.x, boxelSize.z, boxelSize.y}));
		// deactivate all the valid past voxels
		this->setVoxel(start, start + boxelSize, false);
	}
}

bool VoxelWorld::isVoxel( vec3ui const& pos ) const {
	if ((pos.x >= this->_worldSize.x) or
		(pos.y >= this->_worldSize.y) or
		(pos.z >= this->_worldSize.z))
			throw std::runtime_error("voxel position out of world!");

	return this->_grid[pos.x + pos.y * this->_worldSize.x + pos.z * this->_worldSize.x * this->_worldSize.y];
}

bool VoxelWorld::isVoxel( uint32_t x, uint32_t y, uint32_t z ) const {
	if ((x >= this->_worldSize.x) or
		(y >= this->_worldSize.y) or
		(z >= this->_worldSize.z))
			throw std::runtime_error("voxel position out of world!");

	return this->_grid[x + y * this->_worldSize.x + z * this->_worldSize.x * this->_worldSize.y];
}

void VoxelWorld::setVoxel( vec3ui const& pos, bool value ) {
	if ((pos.x >= this->_worldSize.x) or
		(pos.y >= this->_worldSize.y) or
		(pos.z >= this->_worldSize.z))
			throw std::runtime_error("voxel position out of world!");

	this->_grid[pos.x + pos.y * this->_worldSize.x + pos.z * this->_worldSize.x * this->_worldSize.y] = value;
}

void VoxelWorld::setVoxel( vec3ui const& start, vec3ui const& end, bool ) {
	if ((start.x >= this->_worldSize.x) or
		(start.y >= this->_worldSize.y) or
		(start.z >= this->_worldSize.z) or
		(end.x > this->_worldSize.x) or
		(end.y > this->_worldSize.y) or
		(end.z > this->_worldSize.z))
			throw std::runtime_error("voxel position(s) out of world!");

	vec3ui index = start;
	for (index.z=start.z; index.z < end.z; index.z++) {
		for (index.y=start.y; index.y < end.y; index.y++) {
			for (index.x=start.x; index.x < end.x; index.x++)
				this->setVoxel(index, false);
		}
	}
}

void VoxelWorld::setVoxel( uint32_t x, uint32_t y, uint32_t z, bool value ) {
	if ((x >= this->_worldSize.x) or
		(y >= this->_worldSize.y) or
		(z >= this->_worldSize.z))
			throw std::runtime_error("voxel position out of world!");

	this->_grid[x + y * this->_worldSize.x + z * this->_worldSize.x * this->_worldSize.y] = value;
}

vec3ui VoxelWorld::nextVoxel( vec3ui const& pos ) const {
	if ((pos.x >= this->_worldSize.x) or
		(pos.y >= this->_worldSize.y) or
		(pos.z >= this->_worldSize.z))
			throw std::runtime_error("3 voxel position out of world!");

	if (pos.x < this->_worldSize.x - 1)				// next voxel in line
		return vec3ui{pos.x + 1, pos.y, pos.z};
	else if (pos.y < this->_worldSize.y - 1)		// end of line, check y+1
		return vec3ui{0U, pos.y + 1, pos.z};
	else if (pos.z < this->_worldSize.z - 1) 	 	// end of surface, check z+1
		return vec3ui{0U, 0U, pos.z + 1};
	else											// current voxel is the last one, return {0U, 0U, 0U}
		return vec3ui{0U, 0U, 0U};
}


}
