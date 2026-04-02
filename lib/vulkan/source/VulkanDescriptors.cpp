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

VulkanDescriptorSetLayout::VulkanDescriptorSetLayout(VulkanDevice& vulkanDevice, std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings)
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
	poolInfo.flags = poolFlags;	// NB could also be left to hardcoded 0

	if (vkCreateDescriptorPool(vulkanDevice.device(), &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create descriptor pool!");
	}
}

VulkanDescriptorPool::~VulkanDescriptorPool()
{
	vkDestroyDescriptorPool(vulkanDevice.device(), descriptorPool, nullptr);
}

bool	VulkanDescriptorPool::allocateDescriptor(const VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet& descriptor) const
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
	vkFreeDescriptorSets(		// NB!!
		vulkanDevice.device(),
		descriptorPool,
		static_cast<uint32_t>(descriptors.size()),
		descriptors.data());
}

void	VulkanDescriptorPool::resetPool()
{
	vkResetDescriptorPool(vulkanDevice.device(), descriptorPool, 0);
}

VulkanDescriptorWriter::VulkanDescriptorWriter(VulkanDescriptorSetLayout &setLayout, VulkanDescriptorPool &pool, int32_t framesInFlight)
	: setLayout{setLayout}, pool{pool}, framesInFlight{framesInFlight}
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


VulkanDescriptorBinding&	VulkanDescriptorBinding::addBinding(uint32_t binding, VkDescriptorType descriptorType, VkShaderStageFlags stageFlags, uint32_t descriptorCount, void* data)
{
	assert(bindings1.count(binding) == 0 || bindingData.count(binding) == 0 && "Binding already in use");
	VkDescriptorSetLayoutBinding layoutBinding{};
	layoutBinding.binding = binding;
	layoutBinding.descriptorType = descriptorType;
	layoutBinding.descriptorCount = descriptorCount;
	layoutBinding.stageFlags = stageFlags;

	bindings1[binding] = layoutBinding;
	bindingInfo.push_back(layoutBinding);
	bindingData[binding] = data;
}

VulkanDescriptorBinding&	VulkanDescriptorBinding::resetBindings() noexcept
{
	bindings1.clear();
	bindingInfo.clear();
	bindingData.clear();
}


VulkanDescriptorSetFactory::~VulkanDescriptorSetFactory()
{
	vkDestroyDescriptorPool(vulkanDevice.device(), descriptorPool, nullptr);
}

VulkanDescriptorSetFactory&	VulkanDescriptorSetFactory::addPoolSize(VkDescriptorType descriptorType, uint32_t count)
{
	poolSizes.push_back({descriptorType, count});
	return *this;
}

VulkanDescriptorSetFactory&	VulkanDescriptorSetFactory::setPoolFlags(VkDescriptorPoolCreateFlags flags)
{
	poolFlags |= flags;
	return *this;
}

VulkanDescriptorSetFactory&	VulkanDescriptorSetFactory::setMaxSets(uint32_t count)
{
	maxSets = count;
	return *this;
}

VulkanDescriptorSetFactory& VulkanDescriptorSetFactory::createPool()
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
	return *this;
}

void VulkanDescriptorSetFactory::resetPool()
{
	vkResetDescriptorPool(vulkanDevice.device(), descriptorPool, 0);
}

std::unique_ptr<VulkanDescriptorSet> VulkanDescriptorSetFactory::createDescriptorSet(const VulkanDescriptorBinding& bindings)
{
	return std::make_unique<VulkanDescriptorSet>(
		vulkanDevice,
		descriptorPool,
		framesInFlight,
		bindings
	);
}

VulkanDescriptorSet::VulkanDescriptorSet(
	VulkanDevice& vulkanDevice,
	VkDescriptorPool descriptorPool,
	uint32_t framesInFlight,
	const VulkanDescriptorBinding& bindings
) :
	vulkanDevice{vulkanDevice},
	descriptorPool{descriptorPool},
	descriptorSetLayout{VK_NULL_HANDLE},
	descriptorSet{VK_NULL_HANDLE},
	framesInFlight{framesInFlight}
{
	createDescriptorLayout(bindings);
	createDescriptorSet();
	updateDescriptorSet(bindings);
}

VulkanDescriptorSet::~VulkanDescriptorSet()
{
	vkDestroyDescriptorSetLayout(vulkanDevice.device(), descriptorSetLayout, nullptr);
}

void VulkanDescriptorSet::setCurrentFrame(int32_t framesInFlight) noexcept
{

}

void VulkanDescriptorSet::updateUbo(int32_t binding, void* data)
{

}

void VulkanDescriptorSet::bind()
{

}

void VulkanDescriptorSet::createDescriptorLayout(const VulkanDescriptorBinding& bindings)
{
	VkDescriptorSetLayoutCreateInfo	descriptorSetLayoutInfo;
	descriptorSetLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	descriptorSetLayoutInfo.bindingCount = bindings.getNbindings();
	descriptorSetLayoutInfo.pBindings = bindings.getBindingInfo().data();

	if (vkCreateDescriptorSetLayout(
		vulkanDevice.device(),
		&descriptorSetLayoutInfo,
		nullptr,
		&descriptorSetLayout) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create descriptor set layout");
	}
}

void VulkanDescriptorSet::createDescriptorSet()
{
	assert(descriptorSetLayout == VK_NULL_HANDLE && "Descriptor set layout not initialized");

	VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = descriptorPool;
	allocInfo.pSetLayouts = &descriptorSetLayout;
	allocInfo.descriptorSetCount = 1;

	if (vkAllocateDescriptorSets(vulkanDevice.device(), &allocInfo, &descriptorSet) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create descriptor set");
	}
}

void VulkanDescriptorSet::updateDescriptorSet(const VulkanDescriptorBinding& bindings)
{
	assert(descriptorSetLayout == VK_NULL_HANDLE && "Descriptor set layout not initialized");
	assert(descriptorSet == VK_NULL_HANDLE && "Descriptor set not initialized");

	const std::vector<VkDescriptorSetLayoutBinding>& bindInfo = bindings.getBindingInfo();
	std::vector<VkWriteDescriptorSet> writes(bindInfo.size());

	for (uint32_t i = 0U; i < bindInfo.size(); i++)
	{
		writes[i].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writes[i].dstBinding = bindInfo[i].binding;
		writes[i].descriptorType = bindInfo[i].descriptorType;
		writes[i].descriptorCount = 1;
		writes[i].dstSet = descriptorSet;

		void* bindingInfoData = bindings.getBindingData(bindInfo[i].binding);
		if (bindInfo[i].descriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER)
		{
			writes[i].pBufferInfo = static_cast<VkDescriptorBufferInfo*>(bindingInfoData);
			addBuffer(bindInfo[i].binding, writes[i].pBufferInfo->range);
		}
		else if (bindInfo[i].descriptorType == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER)
		{
			writes[i].pImageInfo = static_cast<VkDescriptorImageInfo*>(bindingInfoData);
		}
	}

	vkUpdateDescriptorSets(vulkanDevice.device(), writes.size(), writes.data(), 0, nullptr);
}

void VulkanDescriptorSet::addBuffer(uint32_t binding, uint32_t sizeBuffer)
{
	buffers[binding].reserve(framesInFlight);
	for (uint32_t i = 0; i < framesInFlight; i++)
	{
		buffers[binding][i] = std::make_unique<ve::VulkanBuffer>(
			vulkanDevice.device(),
			sizeBuffer,
			1,
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
		);
		buffers[binding][i]->map();
	}
}


}	// namespace ve
