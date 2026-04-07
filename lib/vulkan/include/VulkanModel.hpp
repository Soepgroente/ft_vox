#pragma once

#include "VulkanBuffer.hpp"
#include "VulkanDevice.hpp"
#include "VulkanUtils.hpp"

#include "Vectors.hpp"

#include <cassert>
#include <cstring>
#include <memory>
#include <vector>
#include <unordered_map>

namespace ve {

inline constexpr uint32_t	VERTEX_PER_VOXEL = 24U;	// number of vertexes per voxel
inline constexpr uint32_t	INDEX_PER_VOXEL = 36U;	// number of vertex indexes per voxel

struct	BoundingBox
{
	vec3	min;
	vec3	max;
};

class VulkanModel
{
	public:

	struct Vertex
	{
		vec3	pos;
		vec3	normal;
		vec2	textureUv;

		static std::vector<VkVertexInputBindingDescription>		getBindingDescriptions();
		static std::vector<VkVertexInputAttributeDescription>	getAttributeDescriptions();

		bool operator==(const Vertex& other) const noexcept
		{
			return	pos == other.pos &&
					normal == other.normal &&
					textureUv == other.textureUv;
		}
		bool operator!=(const Vertex& other) const noexcept
		{
			return !(*this == other);
		}
		bool operator<(const Vertex& other) const noexcept
		{
			if (pos != other.pos)
				return pos < other.pos;
			if (normal != other.normal)
				return normal < other.normal;
			return textureUv < other.textureUv;
		}
	};

	struct Builder
	{
		public:
			std::vector<Vertex>		vertices{};
			std::vector<uint32_t>	indices{};

			void loadModel(const std::string& filepath);
			void emptyData( void ) noexcept;
	};

	VulkanModel() = delete;
	VulkanModel(VulkanDevice& device, const Builder& builder);
	VulkanModel(VulkanDevice& device, const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices);
	VulkanModel(VulkanDevice& device, const std::vector<std::vector<Vertex>>& vertices, const std::array<uint32_t, INDEX_PER_VOXEL>& indexesVoxel);
	~VulkanModel() noexcept = default;

	VulkanModel(const VulkanModel&) = delete;
	VulkanModel& operator=(const VulkanModel&) = delete;

	void	bind(VkCommandBuffer commandBuffer);
	void	draw(VkCommandBuffer commandBuffer);
	void	setName(const std::string& name) { this->name = name; }
	void	setBoundingBox(const std::vector<Vertex>& vertices) noexcept;

	const vec3&	getVertexCenter() const noexcept { return vertexCenter; }
	const vec3&	getBoundingCenter() const noexcept { return boundingCenter; }
	const BoundingBox&	getBoundingBox() const noexcept { return boundingBox; }

	private:
	
	std::string			name;
	bool				hasIndexBuffer = false;
	
	VulkanDevice&		vulkanDevice;
	uint32_t			vertexCount;
	
	std::unique_ptr<VulkanBuffer>	vertexBuffer;
	std::unique_ptr<VulkanBuffer>	indexBuffer;
	
	uint32_t			indexCount;
	
	vec3			vertexCenter;
	vec3			boundingCenter;
	BoundingBox		boundingBox;
	
	void	setObjectCenter() noexcept;

	// void	createVertexBuffers(const std::vector<Vertex>& vertices);
	void	createIndexBuffers(const std::vector<uint32_t>& indices);
	void	createVertexIndexBuffers(const std::vector<std::vector<Vertex>>& vertexes, const std::array<uint32_t, INDEX_PER_VOXEL>& indexesVoxel);
	
	static vec3	calculateVertexCenter(const std::vector<Vertex>& vertices) noexcept;
};

}	// namespace ve

namespace std {

template<>
struct hash<ve::VulkanModel::Vertex>
{
	size_t operator()(ve::VulkanModel::Vertex const& vertex) const
	{
		size_t seed = 0;

		ve::hashCombine(seed, vertex.pos, vertex.normal, vertex.textureUv);
		return seed;
	}
};

}	// namespace std