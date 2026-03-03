#include "VulkanDescriptors.hpp"

// std
#include <cassert>
#include <stdexcept>

namespace ve {

VulkanDescriptorSetLayout::Builder&	VulkanDescriptorSetLayout::Builder::addBinding(
	uint32_t binding,
	VkDescriptorType descriptorType,
	VkShaderStageFlags stageFlags,
	uint32_t count)
{
	assert(bindings.count(binding) == 0 && "Binding already in use");
	VkDescriptorSetLayoutBinding layoutBinding{};

	layoutBinding.binding = binding;
	layoutBinding.descriptorType = descriptorType;
	layoutBinding.descriptorCount = count;
	layoutBinding.stageFlags = stageFlags;
	bindings[binding] = layoutBinding;
	return *this;
}

std::unique_ptr<VulkanDescriptorSetLayout>	VulkanDescriptorSetLayout::Builder::build() const
{
	return std::make_unique<VulkanDescriptorSetLayout>(vulkanDevice, bindings);
}

VulkanDescriptorSetLayout::VulkanDescriptorSetLayout(
	VulkanDevice& vulkanDevice, std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings)
	: vulkanDevice{vulkanDevice}, bindings{bindings}
{
	std::vector<VkDescriptorSetLayoutBinding>	setLayoutBindings{};
	VkDescriptorSetLayoutCreateInfo				descriptorSetLayoutInfo{};

	for (std::pair<uint32_t, VkDescriptorSetLayoutBinding> kv : bindings)
	{
		setLayoutBindings.push_back(kv.second);
	}
	descriptorSetLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	descriptorSetLayoutInfo.bindingCount = static_cast<uint32_t>(setLayoutBindings.size());
	descriptorSetLayoutInfo.pBindings = setLayoutBindings.data();

	if (vkCreateDescriptorSetLayout(
			vulkanDevice.device(),
			&descriptorSetLayoutInfo,
			nullptr,
			&descriptorSetLayout) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create descriptor set layout!");
	}
}

VulkanDescriptorSetLayout::~VulkanDescriptorSetLayout()
{
	vkDestroyDescriptorSetLayout(vulkanDevice.device(), descriptorSetLayout, nullptr);
}

VulkanDescriptorPool::Builder&	VulkanDescriptorPool::Builder::addPoolSize(VkDescriptorType descriptorType, uint32_t count)
{
	poolSizes.push_back({descriptorType, count});
	return *this;
}

VulkanDescriptorPool::Builder&	VulkanDescriptorPool::Builder::setPoolFlags(VkDescriptorPoolCreateFlags flags)
{
	poolFlags = flags;
	return *this;
}

VulkanDescriptorPool::Builder&	VulkanDescriptorPool::Builder::setMaxSets(uint32_t count)
{
	maxSets = count;
	return *this;
}

std::unique_ptr<VulkanDescriptorPool>	VulkanDescriptorPool::Builder::build() const
{
	return std::make_unique<VulkanDescriptorPool>(vulkanDevice, maxSets, poolFlags, poolSizes);
}

VulkanDescriptorPool::VulkanDescriptorPool(
	VulkanDevice& vulkanDevice,
	uint32_t maxSets,
	VkDescriptorPoolCreateFlags poolFlags,
	const std::vector<VkDescriptorPoolSize>& poolSizes)
	: vulkanDevice{vulkanDevice}
{
	VkDescriptorPoolCreateInfo poolInfo{};

	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
	poolInfo.pPoolSizes = poolSizes.data();
	poolInfo.maxSets = maxSets;
	poolInfo.flags = poolFlags;

	if (vkCreateDescriptorPool(vulkanDevice.device(), &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create descriptor pool!");
	}
}

VulkanDescriptorPool::~VulkanDescriptorPool()
{
	vkDestroyDescriptorPool(vulkanDevice.device(), descriptorPool, nullptr);
}

bool	VulkanDescriptorPool::allocateDescriptor(
	const VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet& descriptor) const
{
	VkDescriptorSetAllocateInfo allocInfo{};

	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = descriptorPool;
	allocInfo.pSetLayouts = &descriptorSetLayout;
	allocInfo.descriptorSetCount = 1;

	// Might want to create a "DescriptorPoolManager" class that handles this case, and builds
	// a new pool whenever an old pool fills up. But this is beyond our current scope
	return vkAllocateDescriptorSets(vulkanDevice.device(), &allocInfo, &descriptor) == VK_SUCCESS;
}

void	VulkanDescriptorPool::freeDescriptors(std::vector<VkDescriptorSet>& descriptors) const
{
	vkFreeDescriptorSets(
		vulkanDevice.device(),
		descriptorPool,
		static_cast<uint32_t>(descriptors.size()),
		descriptors.data());
}

void	VulkanDescriptorPool::resetPool()
{
	vkResetDescriptorPool(vulkanDevice.device(), descriptorPool, 0);
}

VulkanDescriptorWriter::VulkanDescriptorWriter(VulkanDescriptorSetLayout &setLayout, VulkanDescriptorPool &pool)
	: setLayout{setLayout}, pool{pool}
{
}

VulkanDescriptorWriter&	VulkanDescriptorWriter::writeBuffer(uint32_t binding, VkDescriptorBufferInfo* bufferInfo)
{
	assert(setLayout.bindings.count(binding) == 1 && "Layout does not contain specified binding");

	VkDescriptorSetLayoutBinding&	bindingDescription = setLayout.bindings[binding];

	assert(bindingDescription.descriptorCount == 1 && "Binding single descriptor info, but binding expects multiple");

	VkWriteDescriptorSet write{};

	write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	write.descriptorType = bindingDescription.descriptorType;
	write.dstBinding = binding;
	write.pBufferInfo = bufferInfo;
	write.descriptorCount = 1;
	writes.push_back(write);

	return *this;
}

VulkanDescriptorWriter &VulkanDescriptorWriter::writeImage(uint32_t binding, VkDescriptorImageInfo* imageInfo)
{
	assert(setLayout.bindings.count(binding) == 1 && "Layout does not contain specified binding");

	VkDescriptorSetLayoutBinding&	bindingDescription = setLayout.bindings[binding];

	assert(bindingDescription.descriptorCount == 1 && "Binding single descriptor info, but binding expects multiple");

	VkWriteDescriptorSet write{};

	write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	write.descriptorType = bindingDescription.descriptorType;
	write.dstBinding = binding;
	write.pImageInfo = imageInfo;
	write.descriptorCount = 1;
	writes.push_back(write);

	return *this;
}

bool	VulkanDescriptorWriter::build(VkDescriptorSet& set)
{
	if (pool.allocateDescriptor(setLayout.getDescriptorSetLayout(), set) == false)
	{
		return false;
	}
	overwrite(set);
	return true;
}

void	VulkanDescriptorWriter::overwrite(VkDescriptorSet& set)
{
	for (VkWriteDescriptorSet& write : writes)
	{
		write.dstSet = set;
	}
	vkUpdateDescriptorSets(pool.vulkanDevice.device(), writes.size(), writes.data(), 0, nullptr);
}

}	// namespace ve
