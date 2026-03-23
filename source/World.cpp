#include "World.hpp"
#include "Config.hpp"
#include "Vox.hpp"

#include <chrono>
#include <mutex>
#include <atomic>
#include <thread>

namespace vox {

/**
 * Get the vertex+texture+normal coordinates of a voxel. The texture coordinates are supposed to
 * apply a whole texture on every face of the voxel
 *
 * @param relativeOrigin The origin of the voxel in a 3D space, the coordinates are relative
 * from such position (default is (0.0f, 0.0f, 0.0f))
 *
 * @return a vector of 24 (fixed number of vertexes per voxel) 
 * instances of ve::VulkanModel::Vertex
 */
VertexVector getVertexRelativeMonoTexture( vec3 const& relativeOrigin ) {
	VertexVector voxelVertexes(VERTEX_PER_VOXEL);
	for (uint32_t i=0; i<VERTEX_PER_VOXEL; i++) {
		// add 0.5 (half size of a voxel) of every coor so that the position is in the exact center
		voxelVertexes[i].pos.x = VOXEL_VERTEXES[i].pos.x + VOXEL_SIZE * 0.5f + relativeOrigin.x;
		voxelVertexes[i].pos.y = VOXEL_VERTEXES[i].pos.y + VOXEL_SIZE * 0.5f + relativeOrigin.y;
		voxelVertexes[i].pos.z = VOXEL_VERTEXES[i].pos.z + VOXEL_SIZE * 0.5f + relativeOrigin.z;
		voxelVertexes[i].normal = VOXEL_VERTEXES[i].normal;
		voxelVertexes[i].textureUv = VOXEL_VERTEXES[i].textureUv;
	}
	return voxelVertexes;
}

/**
 * Get the vertex+texture+normal coordinates of a voxel. The texture coordinates are supposed to
 * apply an atlas so that each face of the voxel has a different texture
 * This atlas is used (file textures/texture_dirt_atlas.jpeg)
 *  _______________
 * |   | B |   |   |	
 * |___|___|___|___|
 * | L | T | R | B |
 * |___|___|___|___|
 * |   | F |   |   |
 * |___|___|___|___|
 *
 * @param relativeOrigin The origin of the voxel in a 3D space, the coordinates are relative
 * from such position (default is (0.0f, 0.0f, 0.0f))
 *
 * @return a vector of 24 (fixed number of vertexes per voxel) 
 * instances of ve::VulkanModel::Vertex
 */
VertexVector getVertexRelativeAtlasTexture( vec3 const& relativeOrigin ) {
	VertexVector voxelVertexes = getVertexRelativeMonoTexture(relativeOrigin);

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

/**
 * Get the the indexes of every face of the voxel. Voxel has 6 faces, every face is made 
 * by 2 triangles, so a total of 36 indexes, drawing the triangles CW order
 *
 * @param start the starting value of the face indexes (default is 0)
 *
 * @return a vector of 36 uin32_t starting from the offset value
 */
IndexVector getIndexRelative( uint32_t start ) {
	IndexVector indexes(INDEX_PER_VOXEL);

	for (uint32_t i=0; i<INDEX_PER_VOXEL; i++)
		indexes[i] = start + VOXEL_VERTEX_INDEXES[i];
	return indexes;
}


/**
 * Build a chunk of voxels (i.e. a World) in a 3D space
 * 
 * @todo currently such voxel generation is static, ideally perlin noise shall be used
 *
 * @param worldPos 3D position of the chunk (bottom-left-front corner), worldPos is
 * the relative origin of the chunk/world, not its center
 *
 * @param worldSize 3D size of the chunk
 */
World::World( vec3i const& worldPos, vec3ui const& worldSize ) : worldPos(worldPos), worldSize(worldSize) {
	vec3 const& relativePos = vec3{
		static_cast<float>(this->worldPos.x) * static_cast<float>(this->worldSize.x),
		static_cast<float>(this->worldPos.y) * static_cast<float>(this->worldSize.y),
		static_cast<float>(this->worldPos.z) * static_cast<float>(this->worldSize.z)
	};

	static ui32 seed = 0;
	// std::cout << "World size: " << worldSize << std::endl;
	// std::cout << "World position: " << worldPos << std::endl;
	// std::cout << "Relative position: " << relativePos << std::endl;
	(void)relativePos;
	for (ui32 z = 0; z < worldSize.z; z++)
	{
		for (ui32 x = 0; x < worldSize.x; x++)
		{
			float noiseValue = perlin(
				(relativePos.x + static_cast<float>(x)) * Config::noiseScalar,
				(relativePos.z + static_cast<float>(z)) * Config::noiseScalar,
				static_cast<float>(seed));
			// float noiseValue = randomNoise(x, z, seed);
			// if (x == 5)
			// {
			// 	std::cout << "Noise value: " << noiseValue << std::endl;
			// 	std::cout << "x: " << relativePos.x + static_cast<float>(x) << std::endl;
			// 	std::cout << "z: " << relativePos.z + static_cast<float>(z) << std::endl;
			// }
			float heightValue = (noiseValue + 1.0f) * 0.5f * static_cast<float>(worldSize.y);
			for (ui32 y = 0; y < static_cast<ui32>(heightValue); y++)
			{
				vec3 centerVoxel{
					static_cast<float>(x + relativePos.x),
					static_cast<float>(heightValue - 0.5f),
					static_cast<float>(z + relativePos.z)
				};
				VertexVector voxelVertexes = getVertexRelativeAtlasTexture(centerVoxel);
				this->vertexes.insert(this->vertexes.end(), voxelVertexes.begin(), voxelVertexes.end());
			}
		}
	}
	this->updateLastAccess();
}

/**
 * Get the weight of the chunk/world: it combines to parameters: 1. the distance from the world
 * from the origin, the bigger the distance, the smaller the weight; 2. the delta time between 
 * now() and the last time the chunk was visited, the bigger the delta the smaller the weight
 *
 * @param origin position from which the distance to the chunk will be calculated
 *
 * @return the combination of distance weight and time weight: the bigger the number
 * the more likely the chunk of voxels will be discarded if the memory limit is reached
 */
float World::getWeight( vec3i const& origin ) const noexcept {
	// NB horizontal distance would be much more important than vertical distance
	uint32_t distance = vec3i::distance1D(this->worldPos, origin);

	Time now = Clock::now();
	uint32_t deltaTime = std::chrono::duration<float, std::chrono::seconds::period>(now - this->lastAccess).count();

	return World::ALPHA * distance + World::BETA * deltaTime;
}

/**
 * When the chunk/world is visited, updates the the last access time
 */
void World::updateLastAccess( void ) noexcept {
	this->lastAccess = Clock::now();
}


/**
 * Spawns new chunks around the player position, in each of these 9 quadrants:
 *  __ __ __
 * |NW|N |NE|
 * |__|__|__|
 * | W| M| E|
 * |__|__|__|
 * |SW|S |SE|
 * |__|__|__|
 * 
 * @param start current position of the player
 *
 * @return true/false if some new chunks are actually generated
 */

bool WorldNavigator::spawnCloseByWorlds( vec3 const& start ) {
	vec3i playerPos = this->worldPosFromPlayerPos(start);
	this->currentWorldPos = playerPos;
	Stopwatch timer;
	bool reloadData = false;

	reloadData |= this->addNewWorld(vec3i{playerPos.x - 1, playerPos.y, playerPos.z - 1});
	reloadData |= this->addNewWorld(vec3i{playerPos.x, playerPos.y, playerPos.z - 1});
	reloadData |= this->addNewWorld(vec3i{playerPos.x + 1, playerPos.y, playerPos.z - 1});
	reloadData |= this->addNewWorld(vec3i{playerPos.x - 1, playerPos.y, playerPos.z});
	reloadData |= this->addNewWorld(playerPos);
	reloadData |= this->addNewWorld(vec3i{playerPos.x + 1, playerPos.y, playerPos.z});
	reloadData |= this->addNewWorld(vec3i{playerPos.x - 1, playerPos.y, playerPos.z + 1});
	reloadData |= this->addNewWorld(vec3i{playerPos.x, playerPos.y, playerPos.z + 1});
	reloadData |= this->addNewWorld(vec3i{playerPos.x + 1, playerPos.y, playerPos.z + 1});

	timer.stop();
	std::cout << timer;
	return reloadData;
}

bool WorldNavigator::spawnCloseByWorlds(vec3 const& start, ThreadManager& threads)
{
	vec3i playerPos = this->worldPosFromPlayerPos(start);
	this->currentWorldPos = playerPos;
	const std::array<vec3i, 9> positions = {{
		{playerPos.x,     playerPos.y, playerPos.z},
		{playerPos.x + 1, playerPos.y, playerPos.z},
		{playerPos.x - 1, playerPos.y, playerPos.z + 1},
		{playerPos.x,     playerPos.y, playerPos.z + 1},
		{playerPos.x + 1, playerPos.y, playerPos.z + 1},
	}};
	Stopwatch timer;
	std::array<std::future<bool>, 9> futures;
	bool reloadData = false;

	for (size_t i = 0; i < positions.size(); i++)
	{
		futures[i] = threads.enqueue([this, pos = positions[i]]() { return this->addNewWorld(pos); });
		futures[i] = threads.enqueue([this, pos = positions[i]]() { return this->addNewWorld(pos); });
	}
	for (std::future<bool>& result : futures)
	{
		reloadData |= result.get();
	}
	timer.stop();
	std::cout << timer;
	return reloadData;
}

/**
 * @return the size of memory, in bytes, used by the chunks
 */
size_t WorldNavigator::getMemoryUsed( void ) const noexcept {
	size_t size = 0U;
	size += this->totVoxels * VERTEX_PER_VOXEL * sizeof(ve::VulkanModel::Vertex);
	size += this->totVoxels * INDEX_PER_VOXEL * sizeof(uint32_t);
	return size;
}

/**
 * Check if a new world is accessed by comparing the position stored in the
 * navigator with the current player position. If the world position actually changed
 * WorldNavigator::spawnCloseByWorlds() shall be called to generate other chunks around
 * the new position
 *
 * @param currentPos 3D player position
 *
 * @param true/false depending in the world position changed
 */
bool WorldNavigator::borderCrossed( vec3 const& currentPos ) const noexcept {
	vec3i playerPos = this->worldPosFromPlayerPos(currentPos);
	return playerPos != this->currentWorldPos;
}

/**
 * Creates a new ve::VulkanModel, that loads vertex data into the GPU. It shall be called everytime
 * a new world/chunks is created (i.e. whenever WorldNavigator::spawnCloseByWorlds() returns true)
 *
 * @param device vulkan object used to build the buffers
 *
 * @return pointer to the newly created model 
 */
std::unique_ptr<ve::VulkanModel> WorldNavigator::createNewModel( ve::VulkanDevice& device ) const {
	std::vector<VertexVector const*>	vertexes(this->worlds.size());
	uint32_t i = 0U;
	for (auto const& [pos , _] : this->worlds)
		vertexes[i++] = &this->worlds.at(pos).getVertexes();
	return std::make_unique<ve::VulkanModel>(device, vertexes, VOXEL_VERTEX_INDEXES);
}

/**
 * By checking if the key worldPos exists or not inside the map of worlds, creates a new
 * chunk/world if it doesn't exist, or just updates the access time if it does. Memory is fixed,
 * so if the limit is reached the 'oldest' (see World::getWeight()) world is deleted.
 *
 * @param worldPos origin of the new chunk of voxels to spawn
 *
 * @return true/false if new data was actually generated
 */

bool WorldNavigator::addNewWorld( vec3i const& worldPos ) {
	if (this->worlds.find(worldPos) != this->worlds.end()) {
		this->worlds[worldPos].updateLastAccess();
		return false;
	} else {
		this->worlds.emplace(worldPos, World(worldPos, this->worldSize));
		this->totVoxels += this->worlds[worldPos].getVertexSize() / VERTEX_PER_VOXEL;

		if (this->worlds.size() > MAX_WORLDS) {
			vec3i furthestWorld = this->findFurthestWorld();
			this->totVoxels -= this->worlds[furthestWorld].getVertexSize() / VERTEX_PER_VOXEL;
			this->worlds.erase(furthestWorld);
		}
		return true;
	}
}

/**
 * Looks up on every existing world and returns the one with the hightest weight, i.e. the one
 * that will be discarder
 *
 * @return origin of the 'oldest' world
 */
vec3i WorldNavigator::findFurthestWorld( void ) const noexcept {
	vec3i furthestWorld = this->currentWorldPos;
	float furthestDist = 0.0f, dist = 0.0f;
	for (auto& [pos, world] : this->worlds) {
		dist = world.getWeight(this->currentWorldPos);
		if (dist > furthestDist) {
			furthestWorld = pos;
			furthestDist = dist;
		}
	}
	return furthestWorld;
}

/**
 * Convert the player position in a 3D continuous space, to the discrete position
 * of the world it belongs
 *
 * @return origin of world the player is in
 */
vec3i WorldNavigator::worldPosFromPlayerPos( vec3 const& localPos ) const noexcept {
	vec3i WorldPos{
		static_cast<int32_t>(localPos.x) / static_cast<int32_t>(this->worldSize.x),
		static_cast<int32_t>(localPos.y) / static_cast<int32_t>(this->worldSize.y),
		static_cast<int32_t>(localPos.z) / static_cast<int32_t>(this->worldSize.z)
	};
	if (localPos.x < 0.0f)
		WorldPos.x -= 1;
	if (localPos.y < 0.0f)
		WorldPos.y -= 1;
	if (localPos.z < 0.0f)
		WorldPos.z -= 1;
	return WorldPos;
}

}	// namespace vox
