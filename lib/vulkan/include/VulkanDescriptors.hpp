#pragma once

#include "VulkanDevice.hpp"
#include "VulkanBuffer.hpp"
#include "VulkanTexture.hpp"

#include <memory>
#include <unordered_map>
#include <vector>
#include <map>


namespace ve {

class VulkanDescriptorSetLayout
{
	public:

	class Builder
	{
		public:

		Builder(VulkanDevice& vulkanDevice) : vulkanDevice{vulkanDevice} {}

		Builder&	addBinding(
			uint32_t binding,
			VkDescriptorType descriptorType,
			VkShaderStageFlags stageFlags,
			uint32_t count = 1
		);
		std::unique_ptr<VulkanDescriptorSetLayout> build() const;

		private:

		VulkanDevice&	vulkanDevice;
		std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings{};
	};

	VulkanDescriptorSetLayout(VulkanDevice& vulkanDevice, std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings);
	~VulkanDescriptorSetLayout();
	VulkanDescriptorSetLayout(const VulkanDescriptorSetLayout&) = delete;
	VulkanDescriptorSetLayout&	operator=(const VulkanDescriptorSetLayout&) = delete;

	VkDescriptorSetLayout getDescriptorSetLayout() const { return descriptorSetLayout; }

	private:

	VulkanDevice&			vulkanDevice;
	VkDescriptorSetLayout	descriptorSetLayout;

	std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings;

	friend class VulkanDescriptorWriter;
};

class VulkanDescriptorPool
{
	public:

	class Builder
	{
		public:

		Builder(VulkanDevice& vulkanDevice) : vulkanDevice{vulkanDevice} {}

		Builder&	addPoolSize(VkDescriptorType descriptorType, uint32_t count);
		Builder&	setPoolFlags(VkDescriptorPoolCreateFlags flags);
		Builder&	setMaxSets(uint32_t count);
		std::unique_ptr<VulkanDescriptorPool>	build() const;

		private:

		VulkanDevice&		vulkanDevice;
		std::vector<VkDescriptorPoolSize>	poolSizes{};
		uint32_t	maxSets = 1000;
		VkDescriptorPoolCreateFlags	poolFlags = 0;
	};

	VulkanDescriptorPool(
		VulkanDevice& vulkanDevice,
		uint32_t maxSets,
		VkDescriptorPoolCreateFlags poolFlags,
		const std::vector<VkDescriptorPoolSize>& poolSizes
	);
	~VulkanDescriptorPool();
	VulkanDescriptorPool(const VulkanDescriptorPool&) = delete;
	VulkanDescriptorPool&	operator=(const VulkanDescriptorPool&) = delete;

	bool	allocateDescriptor(const VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet& descriptor) const;
	void	freeDescriptors(std::vector<VkDescriptorSet>& descriptors) const;
	void	resetPool();

	private:

	VulkanDevice&		vulkanDevice;
	VkDescriptorPool	descriptorPool;

	friend class VulkanDescriptorWriter;
};

class VulkanDescriptorWriter
{
	public:

	VulkanDescriptorWriter(VulkanDescriptorSetLayout& setLayout, VulkanDescriptorPool& pool, int32_t framesInFlight);

	VulkanDescriptorWriter&	writeBuffer(uint32_t binding, VkDescriptorBufferInfo *bufferInfo);
	VulkanDescriptorWriter&	writeImage(uint32_t binding, VkDescriptorImageInfo *imageInfo);

	bool	build(VkDescriptorSet& set);
	void	overwrite(VkDescriptorSet& set);

	private:

	VulkanDescriptorSetLayout&	setLayout;
	VulkanDescriptorPool&		pool;
	std::vector<VkWriteDescriptorSet> writes;
	int32_t						framesInFlight;
	
};


class VulkanDescriptorSet;

struct VulkanBindingEntry
{
	uint32_t			binding;
	VkDescriptorType	descriptorType;
	VkShaderStageFlags	stageFlags;
	uint32_t			count;
	union {
		VkDescriptorBufferInfo	*bufferInfo;
		VkDescriptorImageInfo	*imageInfo;
	};
};

class VulkanDescriptorSetFactory
{
	public:

	VulkanDescriptorSetFactory() = delete;
	VulkanDescriptorSetFactory(VulkanDevice& vulkanDevice, uint32_t framesInFlight) :
		vulkanDevice{vulkanDevice}, framesInFlight{framesInFlight} {};
	~VulkanDescriptorSetFactory();
	VulkanDescriptorSetFactory(const VulkanDescriptorSetFactory&) = delete;
	VulkanDescriptorSetFactory(VulkanDescriptorSetFactory&&) = delete;
	VulkanDescriptorSetFactory& operator=(const VulkanDescriptorSetFactory&) = delete;
	VulkanDescriptorSetFactory& operator=(VulkanDescriptorSetFactory&&) = delete;
	// pool
	VulkanDescriptorSetFactory&	addPoolSize(VkDescriptorType descriptorType, uint32_t count);
	VulkanDescriptorSetFactory&	setPoolFlags(VkDescriptorPoolCreateFlags flags);
	VulkanDescriptorSetFactory&	setMaxSets(uint32_t count);
	// writing
	// VulkanDescriptorSetFactory&	writeBuffer(uint32_t binding, VkDescriptorBufferInfo *bufferInfo);
	// VulkanDescriptorSetFactory&	writeImage(uint32_t binding, VkDescriptorImageInfo *imageInfo);

	void										createPool();
	// std::unique_ptr<VulkanDescriptorSetLayout>	createLayout(const std::vector<VulkanBindingEntry>& bindings);
	std::unique_ptr<VulkanDescriptorSet>		createSet(const std::vector<VulkanBindingEntry>& bindings); // <-- ?

	private:

	VulkanDevice&						vulkanDevice;
	VkDescriptorPool					descriptorPool;
	std::vector<VkDescriptorSetLayout>	descriptorSetLayout;

	std::vector<VkDescriptorPoolSize>	poolSizes{};
	uint32_t							framesInFlight;
	uint32_t							maxSets;
	// VkDescriptorPoolCreateFlags			poolFlags = 0;
	// std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings;
	// std::vector<VkWriteDescriptorSet> writes;
};

class VulkanDescriptorSet
{
	public:

	VulkanDescriptorSet() = delete;
	VulkanDescriptorSet(/* ...*/);
	~VulkanDescriptorSet();
	VulkanDescriptorSet(const VulkanDescriptorSet&) = delete;
	VulkanDescriptorSet(VulkanDescriptorSet&&) = delete;
	VulkanDescriptorSet& operator=(const VulkanDescriptorSet&) = delete;
	VulkanDescriptorSet& operator=(VulkanDescriptorSet&&) = delete;

	void	setCurrentFrame(int32_t framesInFlight) noexcept;
	void	updateUbo(int32_t binding, void* data);
	void	bind();

	private:

	uint32_t										framesInFlight;
	std::map<int32_t,std::vector<VulkanBuffer>>		buffers;
	std::map<int32_t,std::vector<VulkanTexture>>	samplers;
};

}  // namespace ve
