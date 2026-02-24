#include "VoxelWorld.hpp"
#include "Vectors.hpp"

#include <map>


namespace vox {

Boxel::Boxel( vec3 const& pos, vec3 const& size ) {
	this->center = vec3{
		pos.x + size.x / 2.0f,
		pos.y + size.y / 2.0f,
		pos.z + size.z / 2.0f
	};
	// are relative to world coordinates: x: left->right, y: down->up, z: from monitor->user
	this->lengthX = size.x;
	this->lengthY = size.y;
	this->lengthZ = size.z;
}

vec3 const& Boxel::getCenter( void ) const noexcept {
	return this->center;
}

vec3 Boxel::getSize( void ) const noexcept {
	return vec3{this->lengthX, this->lengthY, this->lengthZ};
}

std::vector<vec3> Boxel::getVertexes( void ) const noexcept {
	std::vector<vec3> vertexes(SIZE_VOXEL);
	for (uint32_t i=0; i<SIZE_VOXEL; i++) {
		vec3 relativeVertexPos{
			VOXEL_VERTEXES[i].x * this->lengthX / 2.0f,
			VOXEL_VERTEXES[i].y * this->lengthY / 2.0f,
			VOXEL_VERTEXES[i].z * this->lengthZ / 2.0f,
		};
		vertexes[i] = this->center + relativeVertexPos;
	}
	return vertexes;
}


Voxel::Voxel( vec3 const& pos) {
	this->center = vec3{
		pos.x + Voxel::VOXEL_EDGE / 2.0f,
		pos.y + Voxel::VOXEL_EDGE / 2.0f,
		pos.z + Voxel::VOXEL_EDGE / 2.0f
	};
}

vec3 const& Voxel::getCenter( void ) const noexcept {
	return this->center;
}

std::vector<vec3> Voxel::getVertexes( void ) const noexcept {
	std::vector<vec3> vertexes(SIZE_VOXEL);
	for (uint32_t i=0; i<SIZE_VOXEL; i++)
		vertexes[i] = this->center + VOXEL_VERTEXES[i] * Voxel::VOXEL_EDGE / 2.0f;
	return vertexes;
}


// floor on the ground, two 'towers' of voxels, left and right
std::vector<bool> generatorVoxTest1( uint32_t maxW, uint32_t maxL, uint32_t maxH ) {
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
std::vector<bool> generatorVoxTest2( uint32_t maxW, uint32_t maxL, uint32_t maxH ) {
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
std::vector<bool> generatorVoxTest3( uint32_t maxW, uint32_t maxL, uint32_t maxH ) {
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
std::vector<bool> generatorVoxTest4( uint32_t maxW, uint32_t maxL, uint32_t maxH ) {
	std::vector<bool> grid(maxW * maxL * maxH, false);

	grid[maxW / 2 + (maxL / 2) * maxW + (maxH / 2) * maxW * maxL] = true;
	grid[maxW / 2 + 1 + (maxL / 2) * maxW + (maxH / 2) * maxW * maxL] = true;
	grid[maxW / 2 + (maxL / 2 + 1) * maxW + (maxH / 2) * maxW * maxL] = true;
	grid[maxW / 2 + 1 + (maxL / 2 + 1) * maxW + (maxH / 2) * maxW * maxL] = true;
	return grid;
}

// std::vector<ve::VulkanModel::Vertex> getVoxelVertexes( unsigned char mask, vec3 const& pos ) {
// 	std::vector<ve::VulkanModel::Vertex> vertexes;
// 	vec3 normal = vec3{0.0f, 0.0f, 0.0f};
// 	vec2 textureUv = vec2{0.0f, 0.0f};
//
// 	if (mask & (1 << 7)) {        // insert front-left-top corner
// 		vertexes.push_back(ve::VulkanModel::Vertex{
// 			vec3{
// 				pos.x - VOXEL_EDGE / 2.0f,
// 				pos.y + VOXEL_EDGE / 2.0f,
// 				pos.z - VOXEL_EDGE / 2.0f,
// 			},
// 			ve::generateRandomColor(),
// 			normal,
// 			textureUv
// 		});
// 	}
// 	if (mask & (1 << 6)) {        // insert front-right-top corner
// 		vertexes.push_back(ve::VulkanModel::Vertex{
// 			vec3{
// 				pos.x + VOXEL_EDGE / 2.0f,
// 				pos.y + VOXEL_EDGE / 2.0f,
// 				pos.z - VOXEL_EDGE / 2.0f,
// 			},
// 			ve::generateRandomColor(),
// 			normal,
// 			textureUv
// 		});
// 	}
// 	if (mask & (1 << 5)) {        // insert front-right-bottom corner
// 		vertexes.push_back(ve::VulkanModel::Vertex{
// 			vec3{
// 				pos.x + VOXEL_EDGE / 2.0f,
// 				pos.y - VOXEL_EDGE / 2.0f,
// 				pos.z - VOXEL_EDGE / 2.0f,
// 			},
// 			ve::generateRandomColor(),
// 			normal,
// 			textureUv
// 		});
// 	}
// 	if (mask & (1 << 4)) {        // insert front-left-bottom corner
// 		vertexes.push_back(ve::VulkanModel::Vertex{
// 			vec3{
// 				pos.x - VOXEL_EDGE / 2.0f,
// 				pos.y - VOXEL_EDGE / 2.0f,
// 				pos.z - VOXEL_EDGE / 2.0f,
// 			},
// 			ve::generateRandomColor(),
// 			normal,
// 			textureUv
// 		});
// 	}
// 	if (mask & (1 << 3)) {        // insert back-left-top corner
// 		vertexes.push_back(ve::VulkanModel::Vertex{
// 			vec3{
// 				pos.x - VOXEL_EDGE / 2.0f,
// 				pos.y + VOXEL_EDGE / 2.0f,
// 				pos.z + VOXEL_EDGE / 2.0f,
// 			},
// 			ve::generateRandomColor(),
// 			normal,
// 			textureUv
// 		});
// 	}
// 	if (mask & (1 << 2)) {        // insert back-right-top corner
// 		vertexes.push_back(ve::VulkanModel::Vertex{
// 			vec3{
// 				pos.x + VOXEL_EDGE / 2.0f,
// 				pos.y + VOXEL_EDGE / 2.0f,
// 				pos.z + VOXEL_EDGE / 2.0f,
// 			},
// 			ve::generateRandomColor(),
// 			normal,
// 			textureUv
// 		});
// 	}
// 	if (mask & (1 << 1)) {        // insert back-right-bottom corner
// 		vertexes.push_back(ve::VulkanModel::Vertex{
// 			vec3{
// 				pos.x + VOXEL_EDGE / 2.0f,
// 				pos.y - VOXEL_EDGE / 2.0f,
// 				pos.z + VOXEL_EDGE / 2.0f,
// 			},
// 			ve::generateRandomColor(),
// 			normal,
// 			textureUv
// 		});
// 	}
// 	if (mask & 1) {  		      // insert back-left-bottom corner
// 		vertexes.push_back(ve::VulkanModel::Vertex{
// 			vec3{
// 				pos.x - VOXEL_EDGE / 2.0f,
// 				pos.y - VOXEL_EDGE / 2.0f,
// 				pos.z + VOXEL_EDGE / 2.0f,
// 			},
// 			ve::generateRandomColor(),
// 			normal,
// 			textureUv
// 		});
// 	}
// 	return vertexes;
// }
//
// std::vector<uint32_t> getVoxelIndexes( unsigned short voxelMask, uint32_t startIndex ) {
// 	std::vector<uint32_t> indexes;
// 	// triangle FLT-FRT-FRB
//
// 	// checking front-left-top corner
// 	if (voxelMask & (1 << 10))
// 		indexes.push_back(startIndex++ - (W_WIDTH + 1) * 36);
// 	else if (voxelMask & (1 << 11))
// 		indexes.push_back(startIndex++ - W_WIDTH * 36);
// 	else
// 		indexes.push_back(startIndex++);
// 	// checking front-right-top corner
// 	if (voxelMask & (1 << 11))
// 		indexes.push_back(startIndex++ - W_WIDTH * 36);
// 	else if (voxelMask & (1 << 12))
// 		indexes.push_back(startIndex++ - (W_WIDTH - 1) * 36);
// 	else
// 		indexes.push_back(startIndex++);
// 	// checking front-right-bottom corner
// 	if (voxelMask & (1 << 4))
// 		indexes.push_back(startIndex++ - (W_WIDTH * W_LENGTH) * 36);
// 	else if (voxelMask & (1 << 5))
// 		indexes.push_back(startIndex++ - (W_WIDTH * (W_LENGTH + 1)) * 36);
// 	else if (voxelMask & (1 << 7))
// 		indexes.push_back(startIndex++ - (W_WIDTH * (W_LENGTH - 1)) * 36);
// 	else if (voxelMask & (1 << 8))
// 		indexes.push_back(startIndex++ - (W_WIDTH * (W_LENGTH - 1) + 1) * 36);
// 	else if (voxelMask & (1 << 11))
// 		indexes.push_back(startIndex++ - W_WIDTH * 36);
// 	else if (voxelMask & (1 << 12))
// 		indexes.push_back(startIndex++ - (W_WIDTH - 1) * 36);
// 	else
// 		indexes.push_back(startIndex++);
//
//
// 	// triangle FRT-FRB-FLB
// 	// checking front-right-top corner
// 	if (voxelMask & (1 << 11))
// 		indexes.push_back(startIndex++ - W_WIDTH * 36);
// 	else if (voxelMask & (1 << 12))
// 		indexes.push_back(startIndex++ - (W_WIDTH - 1) * 36);
// 	else
// 		indexes.push_back(startIndex++);
// 	// checking front-right-bottom corner
// 	if (voxelMask & (1 << 4))
// 		indexes.push_back(startIndex++ - (W_WIDTH * W_LENGTH) * 36);
// 	else if (voxelMask & (1 << 5))
// 		indexes.push_back(startIndex++ - (W_WIDTH * (W_LENGTH + 1)) * 36);
// 	else if (voxelMask & (1 << 7))
// 		indexes.push_back(startIndex++ - (W_WIDTH * (W_LENGTH - 1)) * 36);
// 	else if (voxelMask & (1 << 8))
// 		indexes.push_back(startIndex++ - (W_WIDTH * (W_LENGTH - 1) + 1) * 36);
// 	else if (voxelMask & (1 << 11))
// 		indexes.push_back(startIndex++ - W_WIDTH * 36);
// 	else if (voxelMask & (1 << 12))
// 		indexes.push_back(startIndex++ - (W_WIDTH - 1) * 36);
// 	else
// 		indexes.push_back(startIndex++);
// 	// checking front-left-bottom corner
// 	if (voxelMask & (1 << 3))
// 		indexes.push_back(startIndex++ - (W_WIDTH * W_LENGTH - 1) * 36);
// 	else if (voxelMask & (1 << 4))
// 		indexes.push_back(startIndex++ - (W_WIDTH * W_LENGTH) * 36);
// 	else if (voxelMask & (1 << 6))
// 		indexes.push_back(startIndex++ - (W_WIDTH * (W_LENGTH - 1) - 1) * 36);
// 	else if (voxelMask & (1 << 7))
// 		indexes.push_back(startIndex++ - (W_WIDTH * (W_LENGTH - 1)) * 36);
// 	else if (voxelMask & (1 << 9))
// 		indexes.push_back(startIndex++ - 36);
// 	else if (voxelMask & (1 << 10))
// 		indexes.push_back(startIndex++ - (W_WIDTH + 1) * 36);
// 	else if (voxelMask & (1 << 11))
// 		indexes.push_back(startIndex++ - W_WIDTH * 36);
// 	else
// 		indexes.push_back(startIndex++);
//
//
//
//
//
// 	// checking back-left-bottom corner
// 	if (voxelMask & 1)
// 		indexes.push_back(startIndex++ - (W_WIDTH * W_LENGTH + W_WIDTH - 1) * 36);
// 	else if (voxelMask & (1 << 1))
// 		indexes.push_back(startIndex++ - (W_WIDTH * W_LENGTH + W_WIDTH) * 36);
// 	else if (voxelMask & (1 << 3))
// 		indexes.push_back(startIndex++ - (W_WIDTH * W_LENGTH - 1) * 36);
// 	else if (voxelMask & (1 << 4))
// 		indexes.push_back(startIndex++ - (W_WIDTH * W_LENGTH) * 36);
// 	else if (voxelMask & (1 << 9))
// 		indexes.push_back(startIndex++ - 36);
// 	else
// 		indexes.push_back(startIndex++);
// 	// checking back-right-bottom corner
// 	if (voxelMask & (1 << 1))
// 		indexes.push_back(startIndex++ - (W_WIDTH * W_LENGTH + W_WIDTH) * 36);
// 	else if (voxelMask & (1 << 2))
// 		indexes.push_back(startIndex++ - (W_WIDTH * W_LENGTH + W_WIDTH + 1) * 36);
// 	else if (voxelMask & (1 << 4))
// 		indexes.push_back(startIndex++ - (W_WIDTH * W_LENGTH) * 36);
// 	else if (voxelMask & (1 << 5))
// 		indexes.push_back(startIndex++ - (W_WIDTH * W_LENGTH + 1) * 36);
// 	else
// 		indexes.push_back(startIndex++);
// 	// corner back-right-top can't be already in a voxel, so add it directly
// 	indexes.push_back(startIndex++);
// 	// checking back-left-top corner
// 	if (voxelMask & (1 << 9))
// 		indexes.push_back(startIndex++ - 36);
// 	else
// 		indexes.push_back(startIndex++);
// 	// // checking front-left-bottom corner
// 	// if (voxelMask & (1 << 3))
// 	// 	indexes.push_back(startIndex++ - (W_WIDTH * W_LENGTH - 1) * 36);
// 	// else if (voxelMask & (1 << 4))
// 	// 	indexes.push_back(startIndex++ - (W_WIDTH * W_LENGTH) * 36);
// 	// else if (voxelMask & (1 << 6))
// 	// 	indexes.push_back(startIndex++ - (W_WIDTH * (W_LENGTH - 1) - 1) * 36);
// 	// else if (voxelMask & (1 << 7))
// 	// 	indexes.push_back(startIndex++ - (W_WIDTH * (W_LENGTH - 1)) * 36);
// 	// else if (voxelMask & (1 << 9))
// 	// 	indexes.push_back(startIndex++ - 36);
// 	// else if (voxelMask & (1 << 10))
// 	// 	indexes.push_back(startIndex++ - (W_WIDTH + 1) * 36);
// 	// else if (voxelMask & (1 << 11))
// 	// 	indexes.push_back(startIndex++ - W_WIDTH * 36);
// 	// else
// 	// 	indexes.push_back(startIndex++);
// 	// // checking front-right-bottom corner
// 	// if (voxelMask & (1 << 4))
// 	// 	indexes.push_back(startIndex++ - (W_WIDTH * W_LENGTH) * 36);
// 	// else if (voxelMask & (1 << 5))
// 	// 	indexes.push_back(startIndex++ - (W_WIDTH * (W_LENGTH + 1)) * 36);
// 	// else if (voxelMask & (1 << 7))
// 	// 	indexes.push_back(startIndex++ - (W_WIDTH * (W_LENGTH - 1)) * 36);
// 	// else if (voxelMask & (1 << 8))
// 	// 	indexes.push_back(startIndex++ - (W_WIDTH * (W_LENGTH - 1) + 1) * 36);
// 	// else if (voxelMask & (1 << 11))
// 	// 	indexes.push_back(startIndex++ - W_WIDTH * 36);
// 	// else if (voxelMask & (1 << 12))
// 	// 	indexes.push_back(startIndex++ - (W_WIDTH - 1) * 36);
// 	// else
// 	// 	indexes.push_back(startIndex++);
// 	// // checking front-right-top corner
// 	// if (voxelMask & (1 << 11))
// 	// 	indexes.push_back(startIndex++ - W_WIDTH * 36);
// 	// else if (voxelMask & (1 << 12))
// 	// 	indexes.push_back(startIndex++ - (W_WIDTH - 1) * 36);
// 	// else
// 	// 	indexes.push_back(startIndex++);
// 	// // checking front-left-top corner
// 	// if (voxelMask & (1 << 10))
// 	// 	indexes.push_back(startIndex++ - (W_WIDTH + 1) * 36);
// 	// else if (voxelMask & (1 << 11))
// 	// 	indexes.push_back(startIndex++ - W_WIDTH * 36);
// 	// else
// 	// 	indexes.push_back(startIndex++);
//
//		
// 	return indexes;
// 	// return std::vector<uint32_t>{
// 	// 	startIndex + 0, startIndex + 1, startIndex + 2,
// 	// 	startIndex + 0, startIndex + 2, startIndex + 3,
// 	// 	startIndex + 4, startIndex + 7, startIndex + 6,
// 	// 	startIndex + 4, startIndex + 6, startIndex + 5,
// 	// 	startIndex + 0, startIndex + 1, startIndex + 5,
// 	// 	startIndex + 0, startIndex + 5, startIndex + 4,
// 	// 	startIndex + 3, startIndex + 7, startIndex + 6,
// 	// 	startIndex + 3, startIndex + 6, startIndex + 2,
// 	// 	startIndex + 1, startIndex + 2, startIndex + 6,
// 	// 	startIndex + 1, startIndex + 6, startIndex + 5,
// 	// 	startIndex + 0, startIndex + 3, startIndex + 7,
// 	// 	startIndex + 0, startIndex + 7, startIndex + 4,
// 	// };
// }
//
// unsigned char VoxelWorld::checkSurroundings( uint32_t x, uint32_t y, uint32_t z ) const noexcept {
// 	// store which corner of the cube is shared with other voxels
// 	// 8 bits = 8 corners of the cube, from left to right the bits are:
// 	// FLT-FRT-FRB-FLB-BLT-BRT-BRB-BLB  [Front-Back, Left-Right, Top-Bottom]
// 	unsigned char vertexMask = 0b11111111;
// 	// store which voxel surrounds the cube
// 	// 13 bits (ignore leftiest 3) = 13 possible voxel surrounding the voxel checked:
// 	// 9 below (z-1), 4 on the same level ((x-1, y), (x-1, y-1), (x, y-1), (x+1, y-1))
// 	// 
// 	// |X|X|X|
// 	// |X|X|X|
// 	// |X|X|X|
// 	//
// 	// the bits with pos 0->8 correspond to the nine positions of the grid above, 
// 	// starting top left corner, the bits with pos 9->12 correspond to 
// 	// positions (x-1, y), (x-1, y-1), (x, y-1), (x+1, y-1)
// 	unsigned short voxelMask = 0b0000000000000000;
// 	if (z > 0) {	// check level below the voxel
// 		if (this->isVoxel(x, y, z - 1)) {
// 			vertexMask &= 0b11001100;
// 			voxelMask &= 0b0000000000010000;
// 		}
// 		if (y > 0) {
// 			if (this->isVoxel(x, y - 1, z - 1)) {
// 				vertexMask &= 0b11001111;
// 				voxelMask &= 0b0000000001000000;
// 			}
// 		}
// 		if (y < W_LENGTH - 1) {
// 			if (this->isVoxel(x, y + 1, z - 1)) {
// 				vertexMask &= 0b11111100;
// 				voxelMask &= 0b0000000000000010;
// 			}
// 		}
// 		if (x > 0) {
// 			if (this->isVoxel(x - 1, y, z - 1)) {
// 				vertexMask &= 0b11101110;
// 				voxelMask &= 0b0000000000001000;
// 			}
// 			if (y > 0) {
// 				if (this->isVoxel(x - 1, y - 1, z - 1)) {
// 					vertexMask &= 0b11101111;
// 					voxelMask &= 0b0000000001000000;
// 				}
// 			}
// 			if (y < W_LENGTH - 1) {
// 				if (this->isVoxel(x - 1, y + 1, z - 1)) {
// 					vertexMask &= 0b11111110;
// 					voxelMask &= 0b0000000000000001;
// 				}
// 			}
// 		}
// 		if (x < W_WIDTH - 1) {
// 			if (this->isVoxel(x + 1, y, z - 1)) {
// 				vertexMask &= 0b11011101;
// 				voxelMask &= 0b0000000000100000;
// 			}
// 			if (y > 0) {
// 				if (this->isVoxel(x + 1, y - 1, z - 1)) {
// 					vertexMask &= 0b11011111;
// 					voxelMask &= 0b0000000100000000;
// 				}
// 			}
// 			if (y < W_LENGTH) {
// 				if (this->isVoxel(x + 1, y + 1, z - 1)) {
// 					vertexMask &= 0b11111101;
// 					voxelMask &= 0b0000000000000100;
// 				}
// 			}
// 		}
// 	}
// 	// check same level of the voxel
// 	if (x > 0) {
// 		if (this->isVoxel(x - 1, y, z)) {
// 			vertexMask &= 0b01100110;
// 			voxelMask &= 0b0000001000000000;
// 		}
// 		if (y > 0) {
// 			if (this->isVoxel(x - 1, y - 1, z)) {
// 				vertexMask &= 0b01101111;
// 				voxelMask &= 0b0000010000000000;
// 			}
// 		}
// 	}
// 	if (y > 0) {
// 		if (this->isVoxel(x, y - 1, z)) {
// 			vertexMask &= 0b00001111;
// 			voxelMask &= 0b0000100000000000;
// 		}
// 		if (x < W_WIDTH - 1) {
// 			if (this->isVoxel(x + 1, y - 1, z)) {
// 				vertexMask &= 0b10011111;
// 				voxelMask &= 0b0001000000000000;
// 			}
// 		}
// 	}
// 	return vertexMask;
// }

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


VoxelWorld::VoxelWorld( std::vector<bool> (&generator)(uint32_t, uint32_t, uint32_t) ) {
	std::vector<bool> grid = generator(W_WIDTH, W_LENGTH, W_HEIGHT);

	for (uint32_t z=0; z<W_HEIGHT; z++) {
		for (uint32_t y=0; y<W_LENGTH; y++) {
			for (uint32_t x=0; x<W_WIDTH; x++) {
				// order of insetion left->right, bottom->top
				if (grid[x + y * W_WIDTH + z * W_WIDTH * W_LENGTH] == true) {
					float xF = static_cast<float>(x);
					float yF = static_cast<float>(z);		// invert y and z!
					float zF = static_cast<float>(y);
					this->_voxels.push_back(Voxel(vec3{xF, yF, zF}));
				}
			}
		}
	}
	std::cout << "n voxels: " << this->_voxels.size() << std::endl;
}

void VoxelWorld::generateBufferData( void ) {
	std::unordered_map<vec3, uint32_t>	uniqueVertexes;
	uint32_t indexCount = 0U;

	for (Voxel const& voxel : this->_voxels) {
		std::vector<vec3> voxelVertexes = voxel.getVertexes();
		for (uint32_t index : VOXEL_FACES) {
			if (uniqueVertexes.count(voxelVertexes[index]) > 0)
				this->_builder.indices.push_back(uniqueVertexes[voxelVertexes[index]]);
			else {
				uniqueVertexes[voxelVertexes[index]] = indexCount;
				this->_builder.vertices.push_back(ve::VulkanModel::Vertex{
					voxelVertexes[index],
					ve::generateRandomColor(),
					vec3{0.0f, 0.0f, 0.0f},
					vec2{0.0f, 0.0f}
				});
				this->_builder.indices.push_back(indexCount++);
			}
		}

	}
}

void VoxelWorld::generateBufferDataOpt( void ) {
	std::unordered_map<vec3, uint32_t>	uniqueVertexes;
	uint32_t indexCount = 0U;

	for (Boxel const& voxel : this->_boxels) {
		std::vector<vec3> voxelVertexes = voxel.getVertexes();
		for (uint32_t index : VOXEL_FACES) {
			if (uniqueVertexes.count(voxelVertexes[index]) > 0)
				this->_builder.indices.push_back(uniqueVertexes[voxelVertexes[index]]);
			else {
				uniqueVertexes[voxelVertexes[index]] = indexCount;
				this->_builder.vertices.push_back(ve::VulkanModel::Vertex{
					voxelVertexes[index],
					ve::generateRandomColor(),
					vec3{0.0f, 0.0f, 0.0f},
					vec2{0.0f, 0.0f}
				});
				this->_builder.indices.push_back(indexCount++);
			}
		}

	}
}

ve::VulkanModel::Builder const&	VoxelWorld::getBuilder( void ) const noexcept {
	return this->_builder;
}

ve::VulkanModel::Builder&		VoxelWorld::getBuilder( void ) noexcept {
	return this->_builder;
}

}
