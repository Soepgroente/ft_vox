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

enum class ModelType : uint32_t {
    VERTEX = 1 << 0,  // 0001
    NORMAL = 1 << 1,  // 0010
    TEXTURE = 1 << 2,  // 0100
    INDEXED = 1 << 3,  // 1000
};

constexpr ModelType operator|(ModelType a, ModelType b) {
    return static_cast<ModelType>(
        static_cast<uint32_t>(a) | static_cast<uint32_t>(b)
    );
}

constexpr bool operator&(ModelType a, ModelType b) {
    return static_cast<uint32_t>(a) & static_cast<uint32_t>(b);
}

constexpr inline ModelType defaultModelType = ModelType::VERTEX | ModelType::NORMAL | ModelType::TEXTURE;


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
	VulkanModel(VulkanDevice& device, const Builder& builder, ModelType = defaultModelType);
	VulkanModel(VulkanDevice& device, const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices, ModelType = defaultModelType);
	VulkanModel(VulkanDevice& device, const std::vector<vec3>& vertices, const std::vector<uint32_t>& indices, ModelType = ModelType::VERTEX | ModelType::INDEXED);
	VulkanModel(VulkanDevice& device, const std::vector<std::vector<Vertex> const*>& vertices, const std::array<uint32_t, INDEX_PER_VOXEL>& indexesVoxel);
	~VulkanModel() noexcept = default;

	VulkanModel(const VulkanModel&) = delete;
	VulkanModel& operator=(const VulkanModel&) = delete;

	void	bind(VkCommandBuffer commandBuffer);
	void	draw(VkCommandBuffer commandBuffer);
	void	setName(const std::string& name) { this->name = name; }
	void	setBoundingBox(const std::vector<Vertex>& vertices) noexcept;

	std::vector<VkVertexInputBindingDescription>	getBindingDescriptions();
	std::vector<VkVertexInputAttributeDescription>	getAttributeDescriptions();

	const vec3&	getVertexCenter() const noexcept { return vertexCenter; }
	const vec3&	getBoundingCenter() const noexcept { return boundingCenter; }
	const BoundingBox&	getBoundingBox() const noexcept { return boundingBox; }

	private:
	
	std::string			name;
	
	VulkanDevice&		vulkanDevice;
	uint32_t			vertexCount;
	
	std::unique_ptr<VulkanBuffer>	vertexBuffer;
	std::unique_ptr<VulkanBuffer>	indexBuffer;
	
	uint32_t			indexCount;
	
	vec3			vertexCenter;
	vec3			boundingCenter;
	BoundingBox		boundingBox;
	ModelType		type;
	
	void	setObjectCenter() noexcept;

	void	createVertexBuffers(const std::vector<Vertex>& vertices);
	void	createVertexBuffers(const std::vector<vec3>& vertices);
	void	createIndexBuffers(const std::vector<uint32_t>& indices);
	void	createVertexIndexBuffers(const std::vector<std::vector<Vertex> const*>& vertexes, const std::array<uint32_t, INDEX_PER_VOXEL>& indexesVoxel);
	
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