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
 * @param voxelLocation The origin of the voxel in a 3D space, the coordinates are relative
 * from such position (default is (0.0f, 0.0f, 0.0f))
 *
 * @return a vector of 24 (fixed number of vertexes per voxel) 
 * instances of ve::VulkanModel::Vertex
 */

void	addVoxelFace(const vec3& location, VertexVector& chunk, size_t min)
{
	size_t max = min + 4;

	for (size_t i = min; i < max; i++)
	{
		chunk.emplace_back
		(
			ve::VulkanModel::Vertex
			{
				vec3
				{
					VOXEL_VERTEXES[i].pos.x + VOXEL_SIZE * 0.5f + location.x,
					VOXEL_VERTEXES[i].pos.y + VOXEL_SIZE * 0.5f + location.y,
					VOXEL_VERTEXES[i].pos.z + VOXEL_SIZE * 0.5f + location.z
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
void	getVertexRelativeAtlasTexture(const vec3& voxelLocation, VertexVector& chunk)
{
	addVoxelFace(voxelLocation, chunk, static_cast<size_t>(VertexFaces::FRONT));
	addVoxelFace(voxelLocation, chunk, static_cast<size_t>(VertexFaces::BACK));
	addVoxelFace(voxelLocation, chunk, static_cast<size_t>(VertexFaces::LEFT));
	addVoxelFace(voxelLocation, chunk, static_cast<size_t>(VertexFaces::RIGHT));
	addVoxelFace(voxelLocation, chunk, static_cast<size_t>(VertexFaces::TOP));
	addVoxelFace(voxelLocation, chunk, static_cast<size_t>(VertexFaces::BOTTOM));
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
