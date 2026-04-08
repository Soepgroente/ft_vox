#include "World.hpp"
#include "Config.hpp"
#include "Vox.hpp"
#include "VoxelMap.hpp"

#include <iostream>
#include <chrono>
#include <mutex>
#include <set>
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
void	getVertexRelativeMonoTexture(const vec3& relativeOrigin, VertexVector& chunk)
{
	for (ui32 i = 0; i < VERTEX_PER_VOXEL; i++)
	{
		// add 0.5 (half size of a voxel) of every coor so that the position is in the exact center
		chunk.emplace_back
		(
			ve::VulkanModel::Vertex
			{
				vec3
				{
					VOXEL_VERTEXES[i].pos.x + VOXEL_SIZE * 0.5f + relativeOrigin.x,
					VOXEL_VERTEXES[i].pos.y + VOXEL_SIZE * 0.5f + relativeOrigin.y,
					VOXEL_VERTEXES[i].pos.z + VOXEL_SIZE * 0.5f + relativeOrigin.z
				},
			VOXEL_VERTEXES[i].normal,
			VOXEL_VERTEXES[i].textureUv
		});
	}
}

std::vector<vec3> getVertexRelative( vec3 const& relativeOrigin ) {
	std::vector<vec3> voxelVertexes(VERTEX_PER_VOXEL);
	for (uint32_t i=0; i<VERTEX_PER_VOXEL; i++) {
		// add 0.5 (half size of a voxel) of every coor so that the position is in the exact center
		voxelVertexes[i].x = VOXEL_VERTEXES[i].pos.x + VOXEL_SIZE * 0.5f + relativeOrigin.x;
		voxelVertexes[i].y = VOXEL_VERTEXES[i].pos.y + VOXEL_SIZE * 0.5f + relativeOrigin.y;
		voxelVertexes[i].z = VOXEL_VERTEXES[i].pos.z + VOXEL_SIZE * 0.5f + relativeOrigin.z;
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
void	getVertexRelativeAtlasTexture(const vec3& relativeOrigin, VertexVector& chunk)
{
	size_t index = chunk.size();

	getVertexRelativeMonoTexture(relativeOrigin, chunk);

	static constexpr float W = 1.0f / 4.0f;  // width of a tile
	static constexpr float H = 1.0f / 3.0f;  // height of a tile
	static constexpr float padding = 0.004f;

	// front face
	chunk[index + 0].textureUv = vec2{W + padding, 3 * H - padding};
	chunk[index + 0 + 1].textureUv = vec2{2 * W - padding, 3 * H - padding};
	chunk[index + 0 + 2].textureUv = vec2{2 * W - padding, 2 * H + padding};
	chunk[index + 0 + 3].textureUv = vec2{W + padding, 2 * H + padding};
	// back face
	chunk[index + 4].textureUv = vec2{2 * W - padding, padding};
	chunk[index + 4 + 1].textureUv = vec2{W + padding, padding};
	chunk[index + 4 + 2].textureUv = vec2{W + padding, H - padding};
	chunk[index + 4 + 3].textureUv = vec2{2 * W - padding, H - padding};
	// left face
	chunk[index + 8].textureUv = vec2{padding, H + padding};
	chunk[index + 8 + 1].textureUv = vec2{padding, 2 * H - padding};
	chunk[index + 8 + 2].textureUv = vec2{W - padding, 2 * H - padding};
	chunk[index + 8 + 3].textureUv = vec2{W - padding, H + padding};
	// right face
	chunk[index + 12].textureUv = vec2{3 * W - padding, 2 * H - padding};
	chunk[index + 12 + 1].textureUv = vec2{3 * W - padding, H + padding};
	chunk[index + 12 + 2].textureUv = vec2{2 * W + padding, H + padding};
	chunk[index + 12 + 3].textureUv = vec2{2 * W + padding, 2 * H - padding};
	// top face
	chunk[index + 16].textureUv = vec2{W + padding, 2 * H - padding};
	chunk[index + 16 + 1].textureUv = vec2{2 * W - padding, 2 * H - padding};
	chunk[index + 16 + 2].textureUv = vec2{2 * W - padding, H + padding};
	chunk[index + 16 + 3].textureUv = vec2{W + padding, H + padding};
	// back face
	chunk[index + 20].textureUv = vec2{3 * W + padding, H + padding};
	chunk[index + 20 + 1].textureUv = vec2{3 * W + padding, 2 * H - padding};
	chunk[index + 20 + 2].textureUv = vec2{4 * W - padding, 2 * H - padding};
	chunk[index + 20 + 3].textureUv = vec2{4 * W - padding, H + padding};
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

}	// namespace vox
