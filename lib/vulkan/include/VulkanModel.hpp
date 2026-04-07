#pragma once

#include "VulkanBuffer.hpp"
#include "VulkanDevice.hpp"
#include "VulkanUtils.hpp"

#include <memory>
#include <unordered_map>


namespace ve {

inline constexpr uint32_t	VERTEX_PER_VOXEL = 24U;	// number of vertexes per voxel
inline constexpr uint32_t	INDEX_PER_VOXEL = 36U;	// number of vertex indexes per voxel

struct	BoundingBox
{
	vec3	min;
	vec3	max;
};

enum class ModelLayout : uint32_t {
	UNSET = 0,
	VERTEX = 1 << 0,
	NORMAL = 1 << 1,
	TEXTURE = 1 << 2
};

constexpr ModelLayout operator|(ModelLayout a, ModelLayout b) {
	return static_cast<ModelLayout>(
		static_cast<uint32_t>(a) | static_cast<uint32_t>(b)
	);
}

constexpr bool operator&(ModelLayout a, ModelLayout b) {
	return static_cast<uint32_t>(a) & static_cast<uint32_t>(b);
}

constexpr inline ModelLayout DEFAULT_MODEL_LAYOUT = ModelLayout::VERTEX | ModelLayout::NORMAL | ModelLayout::TEXTURE;


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
	VulkanModel(VulkanDevice& device, const Builder& builder, uint32_t binding = 0U, ModelLayout = DEFAULT_MODEL_LAYOUT);
	VulkanModel(VulkanDevice& device, const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices, uint32_t binding = 0U, ModelLayout = DEFAULT_MODEL_LAYOUT);
	VulkanModel(VulkanDevice& device, const std::vector<vec3>& vertices, const std::vector<uint32_t>& indices, uint32_t binding = 0U, ModelLayout = ModelLayout::VERTEX);
	VulkanModel(VulkanDevice& device, const std::vector<std::vector<Vertex> const*>& vertices, const std::array<uint32_t, INDEX_PER_VOXEL>& indexesVoxel, uint32_t binding = 0U, ModelLayout = DEFAULT_MODEL_LAYOUT);
	~VulkanModel() noexcept = default;

	VulkanModel(const VulkanModel&) = delete;
	VulkanModel(VulkanModel&&) = delete;
	VulkanModel& operator=(const VulkanModel&) = delete;
	VulkanModel& operator=(VulkanModel&&) = delete;

	void	bind(VkCommandBuffer commandBuffer);
	void	draw(VkCommandBuffer commandBuffer);
	void	setName(const std::string& name) { this->name = name; }
	void	setBoundingBox(const std::vector<Vertex>& vertices) noexcept;

	std::vector<VkVertexInputBindingDescription>	getBindingDescriptions() const noexcept;
	std::vector<VkVertexInputAttributeDescription>	getAttributeDescriptions() const noexcept;

	const vec3&	getVertexCenter() const noexcept { return vertexCenter; }
	const vec3&	getBoundingCenter() const noexcept { return boundingCenter; }
	const BoundingBox&	getBoundingBox() const noexcept { return boundingBox; }

	private:

	std::string		name;
	VulkanDevice&	vulkanDevice;
	uint32_t		binding;
	ModelLayout		type;
	bool			isIndexed;

	uint32_t		vertexCount;
	uint32_t		indexCount;

	std::unique_ptr<VulkanBuffer>	vertexBuffer;
	std::unique_ptr<VulkanBuffer>	indexBuffer;

	vec3			vertexCenter;
	vec3			boundingCenter;
	BoundingBox		boundingBox;

	void	createVertexBuffers(const std::vector<Vertex>& vertices);
	void	createVertexBuffers(const std::vector<vec3>& vertices);
	void	createIndexBuffers(const std::vector<uint32_t>& indices);
	void	createVertexIndexBuffers(const std::vector<std::vector<Vertex> const*>& vertexes, const std::array<uint32_t, INDEX_PER_VOXEL>& indexesVoxel);

	void	setObjectCenter() noexcept;
	
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