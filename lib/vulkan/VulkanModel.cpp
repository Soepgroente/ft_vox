#include "Vectors.hpp"
#include "VulkanModel.hpp"
#include "VulkanObject.hpp"

#include <iostream>
#include <set>

namespace ve {

VulkanModel::VulkanModel(VulkanDevice& device, const VulkanModel::Builder& builder) : vulkanDevice{device}
{
	createVertexBuffers(builder.vertices);
	createIndexBuffers(builder.indices);
	vertexCount = static_cast<uint32_t>(builder.vertices.size());
	indexCount = static_cast<uint32_t>(builder.indices.size());
}

VulkanModel::~VulkanModel()
{
}

void	VulkanModel::createVertexBuffers(const std::vector<Vertex>& vertices)
{
	vertexCount = static_cast<uint32_t>(vertices.size());

	assert(vertexCount >= 3 && "Vertex count must be at least 3");

	VkDeviceSize	bufferSize = sizeof(vertices[0]) * vertexCount;
	uint32_t		vertexSize = sizeof(vertices[0]);

	VulkanBuffer	stagingBuffer(
		vulkanDevice,
		vertexSize,
		vertexCount,
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
	);

	stagingBuffer.map();
	stagingBuffer.writeToBuffer((void*)vertices.data());

	vertexBuffer = std::make_unique<VulkanBuffer>(
		vulkanDevice,
		vertexSize,
		vertexCount,
		VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
	);
	vulkanDevice.copyBuffer(stagingBuffer.getBuffer(), vertexBuffer->getBuffer(), bufferSize);
}

void	VulkanModel::createIndexBuffers(const std::vector<uint32_t>& indices)
{
	indexCount = static_cast<uint32_t>(indices.size());
	hasIndexBuffer = indexCount > 0;

	if (hasIndexBuffer == false)
	{
		return;
	}

	VkDeviceSize	bufferSize = sizeof(indices[0]) * indexCount;
	uint32_t		indexSize = sizeof(indices[0]);

	VulkanBuffer	stagingBuffer(
		vulkanDevice,
		indexSize,
		indexCount,
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
	);

	stagingBuffer.map();
	stagingBuffer.writeToBuffer((void*)indices.data());

	indexBuffer = std::make_unique<VulkanBuffer>(
		vulkanDevice,
		indexSize,
		indexCount,
		VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
	);
	vulkanDevice.copyBuffer(stagingBuffer.getBuffer(), indexBuffer->getBuffer(), bufferSize);
}

void	VulkanModel::bind(VkCommandBuffer commandBuffer)
{
	VkBuffer		buffers[] = {vertexBuffer->getBuffer()};
	VkDeviceSize	offsets[] = {0};

	vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);
	if (hasIndexBuffer == true)
	{
		vkCmdBindIndexBuffer(commandBuffer, indexBuffer->getBuffer(), 0, VK_INDEX_TYPE_UINT32);
	}
}

void	VulkanModel::draw(VkCommandBuffer commandBuffer)
{
	if (hasIndexBuffer == true)
	{
		vkCmdDrawIndexed(commandBuffer, indexCount, 1, 0, 0, 0);
	}
	else
	{
		vkCmdDraw(commandBuffer, vertexCount, 1, 0, 0);
	}
}

std::vector<VkVertexInputBindingDescription>	VulkanModel::Vertex::getBindingDescriptions()
{
	std::vector<VkVertexInputBindingDescription>	bindingDescriptions(1);

	bindingDescriptions[0].binding = 0;
	bindingDescriptions[0].stride = sizeof(Vertex);
	bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
	return bindingDescriptions;
}

std::vector<VkVertexInputAttributeDescription>	VulkanModel::Vertex::getAttributeDescriptions()
{
	std::vector<VkVertexInputAttributeDescription>	attributeDescriptions;

	attributeDescriptions.reserve(3);

	attributeDescriptions.push_back(
		VkVertexInputAttributeDescription{0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, pos)}
	);
	attributeDescriptions.push_back(
		VkVertexInputAttributeDescription{1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, color)}
	);
	attributeDescriptions.push_back(
		VkVertexInputAttributeDescription{2, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, normal)}
	);
	attributeDescriptions.push_back(
		VkVertexInputAttributeDescription{3, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, textureUv)}
	);
	return attributeDescriptions;
}

void	VulkanModel::setBoundingBox(const std::vector<Vertex>& vertices) noexcept
{
	boundingBox.min.x = std::min_element(vertices.begin(), vertices.end(),
		[](const Vertex& a, const Vertex& b) { return a.pos.x < b.pos.x; })->pos.x;
	boundingBox.max.x = std::max_element(vertices.begin(), vertices.end(),
		[](const Vertex& a, const Vertex& b) { return a.pos.x < b.pos.x; })->pos.x;
	boundingBox.min.y = std::min_element(vertices.begin(), vertices.end(),
		[](const Vertex& a, const Vertex& b) { return a.pos.y < b.pos.y; })->pos.y;
	boundingBox.max.y = std::max_element(vertices.begin(), vertices.end(),
		[](const Vertex& a, const Vertex& b) { return a.pos.y < b.pos.y; })->pos.y;
	boundingBox.min.z = std::min_element(vertices.begin(), vertices.end(),
		[](const Vertex& a, const Vertex& b) { return a.pos.z < b.pos.z; })->pos.z;
	boundingBox.max.z = std::max_element(vertices.begin(), vertices.end(),
		[](const Vertex& a, const Vertex& b) { return a.pos.z < b.pos.z; })->pos.z;
}

void	VulkanModel::setObjectCenter() noexcept
{
	boundingCenter = (boundingBox.min + boundingBox.max) / 2.0f;
}

vec3	VulkanModel::calculateVertexCenter(const std::vector<Vertex>& vertices) noexcept
{
	vec3	center{};

	for (const Vertex& vertex : vertices)
	{
		center += vertex.pos;
	}
	center /= static_cast<float>(vertices.size());
	return center;
}

std::unique_ptr<VulkanModel>	VulkanModel::createModelFromFile(VulkanDevice& device, const std::string& filepath)
{
	Builder	builder{};

	builder.loadModel(filepath);

	if (builder.vertices.size() < 10)
	{
		for (VulkanModel::Vertex& vertex : builder.vertices)
		{
			vertex.color = {0.6f};
		}
	}

	std::unique_ptr<VulkanModel>	model = std::make_unique<VulkanModel>(device, builder);

	model->vertexCenter = model->calculateVertexCenter(builder.vertices);
	model->setBoundingBox(builder.vertices);
	model->setObjectCenter();
	return model;
}

void	VulkanModel::Builder::loadModel(const std::string &filepath)
{
	std::vector<ObjInfo>		objs = parseOBJFile(filepath);
	std::unordered_map<Vertex, uint32_t>	uniqueVertices{};

	vertices.clear();
	indices.clear();

	for (const ObjInfo& obj : objs)
	{
		for (const ObjComponent& component : obj.components)
		{
			for (size_t i = 0; i < component.faceIndices.size(); i++)
			{
				const std::vector<uint32_t>& face = component.faceIndices[i];
				const std::vector<uint32_t>& tex = component.textureIndices[i];
				const std::vector<uint32_t>& norm = component.normalIndices[i];

				if (face.size() < 3)
				{
					continue;
				}

				/*	If more than 3 faces, create triangles	*/

				for (size_t j = 2; j < face.size(); j++)
				{
					for (size_t ti : {0UL, j - 1, j})
					{
						Vertex	vertex{};

						vertex.pos = obj.vertices[face[ti]];
						if (tex.size() > ti)
						{
							vertex.textureUv = obj.textureCoords[tex[ti]];
						}
						else
						{
							vec3 absPos = vec3{std::abs(vertex.pos.x), std::abs(vertex.pos.y), std::abs(vertex.pos.z)};
							vec3 norm = vertex.normal;
							
							if (std::abs(norm.x) > 0.5f)
							{
								vertex.textureUv = vec2{(vertex.pos.z + 1.0f) * 0.5f, (vertex.pos. y + 1.0f) * 0.5f};
							}
							else if (std::abs(norm.y) > 0.5f)
							{
								vertex.textureUv = vec2{(vertex.pos.x + 1.0f) * 0.5f, (vertex.pos.z + 1.0f) * 0.5f};
							}
							else
							{
								vertex.textureUv = vec2{(vertex.pos.x + 1.0f) * 0.5f, (vertex.pos.y + 1.0f) * 0.5f};
							}
						}
						if (norm.size() > ti)
						{
							vertex.normal = obj.normals[norm[ti]];
						}
						vertex.color = generateRandomColor();
						if (uniqueVertices.count(vertex) == 0)
						{
							uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
							vertices.push_back(vertex);
						}
						indices.push_back(uniqueVertices[vertex]);
					}
				}
			}
		}
	}
}

}	// namespace ve