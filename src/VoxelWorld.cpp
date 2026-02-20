#include "VoxelWorld.hpp"
#include "Vectors.hpp"


namespace vox {

// floor on the ground, two 'towers' of voxels, left and right
VoxelGrid generatorVoxTest1( void ) {
	VoxelGrid grid;
	grid.fill(false);
	// set floor
	for( uint32_t j=0; j<W_LENGTH; j++) {
		for( uint32_t i=0; i<W_WIDTH; i++)
			grid[i + j * W_WIDTH] = true;
	}
	// // left tower, 4 voxel base
	for( uint32_t i=1; i<W_HEIGHT; i++) {
		grid[1 + (W_WIDTH - 2) * W_WIDTH + i * W_WIDTH * W_LENGTH] = true;
		grid[2 + (W_WIDTH - 2) * W_WIDTH + i * W_WIDTH * W_LENGTH] = true;
		grid[1 + (W_WIDTH - 1) * W_WIDTH + i * W_WIDTH * W_LENGTH] = true;
		grid[2 + (W_WIDTH - 1) * W_WIDTH + i * W_WIDTH * W_LENGTH] = true;
	}
	// right tower, 4 voxel base
	for( uint32_t i=1; i<W_HEIGHT; i++) {
		grid[5 + (W_WIDTH - 2) * W_WIDTH + i * W_WIDTH * W_LENGTH] = true;
		grid[6 + (W_WIDTH - 2) * W_WIDTH + i * W_WIDTH * W_LENGTH] = true;
		grid[5 + (W_WIDTH - 1) * W_WIDTH + i * W_WIDTH * W_LENGTH] = true;
		grid[6 + (W_WIDTH - 1) * W_WIDTH + i * W_WIDTH * W_LENGTH] = true;
	}

	return grid;
}

// floor on the ground, rest is random
VoxelGrid generatorVoxTest2( void ) {
	VoxelGrid grid;
	grid.fill(false);
	// set floor
	for( uint32_t j=0; j<W_LENGTH; j++) {
		for( uint32_t i=0; i<W_WIDTH; i++)
			grid[i + j * W_WIDTH] = true;
	}
	for( uint32_t k=1; k<W_HEIGHT; k++) {
		for( uint32_t j=0; j<W_LENGTH; j++) {
			for( uint32_t i=0; i<W_WIDTH; i++) {
				bool value = (ve::randomFloat() > 0.85f) ? true : false;
				grid[i + j * W_WIDTH + k * W_WIDTH * W_LENGTH] = value;
			}
		}
	}

	return grid;
}

// basic random generation
VoxelGrid generatorVoxTest3( void ) {
	VoxelGrid grid;
	grid.fill(false);
	// set floor
	for( uint32_t j=0; j<W_LENGTH; j++) {
		for( uint32_t i=0; i<W_WIDTH; i++)
			grid[i + j * W_WIDTH] = true;
	}
	// set ceiling
	for( uint32_t j=0; j<W_LENGTH; j++) {
		for( uint32_t i=0; i<W_WIDTH; i++)
			grid[i + j * W_WIDTH + (W_HEIGHT - 1) * W_WIDTH * W_LENGTH] = true;
	}
	// four columns on corner
	for( uint32_t i=0; i<W_HEIGHT; i++) {
		grid[i * W_LENGTH * W_WIDTH] = true;
		grid[(W_WIDTH - 1) + i * W_LENGTH * W_WIDTH] = true;
		grid[(W_LENGTH - 1) * W_WIDTH + i * W_LENGTH * W_WIDTH] = true;
		grid[(W_WIDTH - 1) + (W_LENGTH - 1) * W_WIDTH + i * W_LENGTH * W_WIDTH] = true;
	}
	// random cubes, with penality along z (the higher the more difficult the spawn)
	for( uint32_t k=1; k<W_HEIGHT - 1; k++) {
		float t = static_cast<float>(k) / W_HEIGHT;
		float factor = 1.0f - 1.0f * t;
		for( uint32_t j=1; j<W_LENGTH-1; j++) {
			for( uint32_t i=1; i<W_WIDTH-1; i++) {
				bool value = (ve::randomFloat() * factor) > 0.3f;
				grid[i + j * W_WIDTH + k * W_WIDTH * W_LENGTH] = value;
			}
		}
	}

	return grid;
}
VoxelWorld	VoxelWorld::createVoxelWorld( VoxelGrid (&generator)() ) {
	VoxelWorld newWorld;
	newWorld._grid = generator();
	return newWorld;
}

std::shared_ptr<ve::VulkanModel> VoxelWorld::generateModel( void ) {
	std::shared_ptr<ve::VulkanModel> model;

	return model;
}

bool VoxelWorld::hasVoxel( uint32_t x, uint32_t y, uint32_t z) const {
	size_t index = static_cast<size_t>(x) + 
					static_cast<size_t>(y) * W_WIDTH + 
					static_cast<size_t>(z) * W_WIDTH * W_LENGTH;

	if (index >= this->_grid.size())
		throw std::runtime_error("Voxel index out of bounds");

	return this->_grid[index];
}

std::vector<vec3> VoxelWorld::generatePositions( void ) {
	std::vector<vec3> positions;
	for (uint32_t z=0; z<W_HEIGHT; z++) {
		for (uint32_t y=0; y<W_LENGTH; y++) {
			for (uint32_t x=0; x<W_WIDTH; x++) {
				// voxel position in world coordinates, invert y and z
				if (this->hasVoxel(x, z, y)) {
					float xF = static_cast<float>(x) - W_WIDTH / 2.0f + 0.5f;
					float yF = static_cast<float>(y);
					float zF = static_cast<float>(z);
					positions.push_back(vec3{xF, yF, zF});
				}
			}
		}
	}
	return positions;
}

std::ostream& operator<<(std::ostream& os, VoxelWorld const& world) {
	for (uint32_t z=0; z<W_HEIGHT; z++) {
		os << "level: " << z << std::endl;
		for (int32_t y=W_LENGTH-1; y>=0; y--) {
			os << "|";
			for (uint32_t x=0; x<W_WIDTH; x++) {
				char isVox = world.hasVoxel(x, y, z) ? 'O' : ' ';
				os << isVox << "|";
			}
			os << std::endl;
		}
		os << std::endl;
	}
	return os;
}

}
