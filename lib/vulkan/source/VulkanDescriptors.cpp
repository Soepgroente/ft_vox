#include "VulkanDescriptors.hpp"

// std
#include <cassert>
#include <stdexcept>

namespace ve {

// VulkanDescriptorSetLayout::Builder&	VulkanDescriptorSetLayout::Builder::addBinding(
// 	uint32_t binding,
// 	VkDescriptorType descriptorType,
// 	VkShaderStageFlags stageFlags,
// 	uint32_t count)
// {
// 	assert(bindings.count(binding) == 0 && "Binding already in use");
// 	VkDescriptorSetLayoutBinding layoutBinding{};
//
// 	layoutBinding.binding = binding;
// 	layoutBinding.descriptorType = descriptorType;
// 	layoutBinding.descriptorCount = count;
// 	layoutBinding.stageFlags = stageFlags;
// 	bindings[binding] = layoutBinding;
// 	return *this;
// }//
//
// std::unique_ptr<VulkanDescriptorSetLayout>	VulkanDescriptorSetLayout::Builder::build() const
// {
// 	return std::make_unique<VulkanDescriptorSetLayout>(vulkanDevice, bindings);
// }
//
// VulkanDescriptorSetLayout::VulkanDescriptorSetLayout(VulkanDevice& vulkanDevice, std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings)
// 	: vulkanDevice{vulkanDevice}, bindings{bindings}
// {
// 	std::vector<VkDescriptorSetLayoutBinding>	setLayoutBindings{};
// 	VkDescriptorSetLayoutCreateInfo				descriptorSetLayoutInfo{};
//
// 	for (std::pair<uint32_t, VkDescriptorSetLayoutBinding> kv : bindings)
// 	{
// 		setLayoutBindings.push_back(kv.second);
// 	}
// 	descriptorSetLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
// 	descriptorSetLayoutInfo.bindingCount = static_cast<uint32_t>(setLayoutBindings.size());
// 	descriptorSetLayoutInfo.pBindings = setLayoutBindings.data();//
//
// 	if (vkCreateDescriptorSetLayout(
// 			vulkanDevice.device(),
// 			&descriptorSetLayoutInfo,
// 			nullptr,
// 			&descriptorSetLayout) != VK_SUCCESS)
// 	{
// 		throw std::runtime_error("failed to create descriptor set layout!");
// 	}
// }//
//
// VulkanDescriptorSetLayout::~VulkanDescriptorSetLayout()
// {
// 	vkDestroyDescriptorSetLayout(vulkanDevice.device(), descriptorSetLayout, nullptr);
// }
//
// VulkanDescriptorPool::Builder&	VulkanDescriptorPool::Builder::addPoolSize(VkDescriptorType descriptorType, uint32_t count)
// {
// 	poolSizes.push_back({descriptorType, count});
// 	return *this;
// }//
//
// VulkanDescriptorPool::Builder&	VulkanDescriptorPool::Builder::setPoolFlags(VkDescriptorPoolCreateFlags flags)
// {
// 	poolFlags = flags;
// 	return *this;
// }//
//
// VulkanDescriptorPool::Builder&	VulkanDescriptorPool::Builder::setMaxSets(uint32_t count)
// {
// 	maxSets = count;
// 	return *this;
// }//
//
// std::unique_ptr<VulkanDescriptorPool>	VulkanDescriptorPool::Builder::build() const
// {
// 	return std::make_unique<VulkanDescriptorPool>(vulkanDevice, maxSets, poolFlags, poolSizes);
// }
//
// VulkanDescriptorPool::VulkanDescriptorPool(
// 	VulkanDevice& vulkanDevice,
// 	uint32_t maxSets,
// 	VkDescriptorPoolCreateFlags poolFlags,
// 	const std::vector<VkDescriptorPoolSize>& poolSizes)
// 	: vulkanDevice{vulkanDevice}
// {
// 	VkDescriptorPoolCreateInfo poolInfo{};
//
// 	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
// 	poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
// 	poolInfo.pPoolSizes = poolSizes.data();
// 	poolInfo.maxSets = maxSets;
// 	poolInfo.flags = poolFlags;	// NB could also be left to hardcoded 0
//
// 	if (vkCreateDescriptorPool(vulkanDevice.device(), &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS)
// 	{
// 		throw std::runtime_error("failed to create descriptor pool!");
// 	}
// }//
//
// VulkanDescriptorPool::~VulkanDescriptorPool()
// {
// 	vkDestroyDescriptorPool(vulkanDevice.device(), descriptorPool, nullptr);
// }
//
// bool	VulkanDescriptorPool::allocateDescriptor(const VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet& descriptor) const
// {
// 	VkDescriptorSetAllocateInfo allocInfo{};//
//
// 	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
// 	allocInfo.descriptorPool = descriptorPool;
// 	allocInfo.pSetLayouts = &descriptorSetLayout;
// 	allocInfo.descriptorSetCount = 1;//
//
// 	// Might want to create a "DescriptorPoolManager" class that handles this case, and builds
// 	// a new pool whenever an old pool fills up. But this is beyond our current scope
// 	return vkAllocateDescriptorSets(vulkanDevice.device(), &allocInfo, &descriptor) == VK_SUCCESS;
// }//
//
// void	VulkanDescriptorPool::freeDescriptors(std::vector<VkDescriptorSet>& descriptors) const
// {
// 	vkFreeDescriptorSets(		// NB!!
// 		vulkanDevice.device(),
// 		descriptorPool,
// 		static_cast<uint32_t>(descriptors.size()),
// 		descriptors.data());
// }//
//
// void	VulkanDescriptorPool::resetPool()
// {
// 	vkResetDescriptorPool(vulkanDevice.device(), descriptorPool, 0);
// }
//
// VulkanDescriptorWriter::VulkanDescriptorWriter(VulkanDescriptorSetLayout &setLayout, VulkanDescriptorPool &pool, int32_t framesInFlight)
// 	: setLayout{setLayout}, pool{pool}, framesInFlight{framesInFlight}
// {
// }//
//
// VulkanDescriptorWriter&	VulkanDescriptorWriter::writeBuffer(uint32_t binding, VkDescriptorBufferInfo* bufferInfo)
// {
// 	assert(setLayout.bindings.count(binding) == 1 && "Layout does not contain specified binding");//
//
// 	VkDescriptorSetLayoutBinding&	bindingDescription = setLayout.bindings[binding];//
//
// 	assert(bindingDescription.descriptorCount == 1 && "Binding single descriptor info, but binding expects multiple");
//
//// 	VkWriteDescriptorSet write{};
//
// 	write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
// 	write.descriptorType = bindingDescription.descriptorType;
// 	write.dstBinding = binding;
// 	write.pBufferInfo = bufferInfo;
// 	write.descriptorCount = 1;
// 	writes.push_back(write);//
//
// 	return *this;
// }
//
// VulkanDescriptorWriter &VulkanDescriptorWriter::writeImage(uint32_t binding, VkDescriptorImageInfo* imageInfo)
// {
// 	assert(setLayout.bindings.count(binding) == 1 && "Layout does not contain specified binding");//
//
// 	VkDescriptorSetLayoutBinding&	bindingDescription = setLayout.bindings[binding];//
//
// 	assert(bindingDescription.descriptorCount == 1 && "Binding single descriptor info, but binding expects multiple");
//
//// 	VkWriteDescriptorSet write{};
//
// 	write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
// 	write.descriptorType = bindingDescription.descriptorType;
// 	write.dstBinding = binding;
// 	write.pImageInfo = imageInfo;
// 	write.descriptorCount = 1;
// 	writes.push_back(write);//
//
// 	return *this;
// }
//
//


VulkanDescriptorSetFactory::~VulkanDescriptorSetFactory()
{
	vkDestroyDescriptorPool(vulkanDevice.device(), descriptorPool, nullptr);
}

VulkanDescriptorSetFactory&	VulkanDescriptorSetFactory::addPoolSize(VkDescriptorType descriptorType, uint32_t count)
{
	VkDescriptorPoolSize poolSize{};
	poolSize.type = descriptorType;
	poolSize.descriptorCount = count;

	poolSizes.push_back(poolSize);
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

VulkanDescriptorSetFactory& VulkanDescriptorSetFactory::resetPool()
{
	vkResetDescriptorPool(vulkanDevice.device(), descriptorPool, 0);
	return *this;
}

VulkanDescriptorSetFactory&	VulkanDescriptorSetFactory::addBinding(uint32_t binding, VkDescriptorType descriptorType, VkShaderStageFlags stageFlags, uint32_t descriptorCount)
{
	for (const VkDescriptorSetLayoutBinding& existingBindings : bindings)
		assert(existingBindings.binding != binding && "Binding already in use");

	VkDescriptorSetLayoutBinding layoutBinding{};
	layoutBinding.binding = binding;
	layoutBinding.descriptorType = descriptorType;
	layoutBinding.descriptorCount = descriptorCount;
	layoutBinding.stageFlags = stageFlags;//

	bindings.push_back(layoutBinding);
	return *this;
}

VulkanDescriptorSetFactory&	VulkanDescriptorSetFactory::resetBindings() noexcept
{
	bindings.clear();
	return *this;
}

std::unique_ptr<VulkanDescriptorSet> VulkanDescriptorSetFactory::createDescriptorSet(VkDescriptorSetLayoutCreateFlags flags)
{
	VkDescriptorSetLayoutCreateInfo	descriptorSetLayoutInfo{};
	descriptorSetLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	descriptorSetLayoutInfo.bindingCount = bindings.size();
	descriptorSetLayoutInfo.pBindings = bindings.data();
	descriptorSetLayoutInfo.flags = flags;

	VkDescriptorSetLayout descriptorSetLayout;
	if (vkCreateDescriptorSetLayout(
		vulkanDevice.device(),
		&descriptorSetLayoutInfo,
		nullptr,
		&descriptorSetLayout) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create descriptor set layout");
	}

	return std::make_unique<VulkanDescriptorSet>(
		vulkanDevice,
		descriptorSetLayout,
		framesInFlight,
		descriptorPool,
		bindings
	);
}


VulkanDescriptorSet::VulkanDescriptorSet(
	VulkanDevice& vulkanDevice,
	VkDescriptorSetLayout descriptorSetLayout,
	uint32_t framesInFlight,
	VkDescriptorPool descriptorPool,
	std::vector<VkDescriptorSetLayoutBinding> bindings
) :
	vulkanDevice{vulkanDevice},
	descriptorSetLayout{descriptorSetLayout},
	framesInFlight{framesInFlight},
	currentFrame{0U}
{
	VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = descriptorPool;
	allocInfo.pSetLayouts = &descriptorSetLayout;
	allocInfo.descriptorSetCount = 1;

	descriptorSets.reserve(framesInFlight);
	for (uint32_t frame = 0U; frame < framesInFlight; frame++)
	{
		if (vkAllocateDescriptorSets(vulkanDevice.device(), &allocInfo, &descriptorSets[frame]) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create descriptor set");
		}
	}

	for (const VkDescriptorSetLayoutBinding& binding : bindings)
	{
		if (binding.descriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER)
		{
			buffers[binding.binding] = std::vector<std::unique_ptr<VulkanBuffer>>(framesInFlight);
		}
		else if (binding.descriptorType == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER)
		{
			textures[binding.binding] = nullptr;
		}
	}
}

VulkanDescriptorSet::~VulkanDescriptorSet()
{
	vkDestroyDescriptorSetLayout(vulkanDevice.device(), descriptorSetLayout, nullptr);
}

void VulkanDescriptorSet::setCurrentFrame(uint32_t frame) noexcept
{
	assert(frame < framesInFlight && "Frame index over limit");
	currentFrame = frame;
}

void VulkanDescriptorSet::updateUbo(int32_t binding, void const* data)
{
	assert(buffers.count(binding) == 0U && "Buffer binding not existent in descriptor set");
	buffers[binding][currentFrame]->writeToBuffer(data);
}

void VulkanDescriptorSet::bind(VkCommandBuffer commandBuffer, const VulkanPipeline& pipeline, uint32_t binding)
{
	vkCmdBindDescriptorSets(
		commandBuffer,
		VK_PIPELINE_BIND_POINT_GRAPHICS,
		pipeline.getPipelineLayout(),
		binding,
		1,
		&descriptorSets[currentFrame],
		0,
		nullptr
	);

}

void VulkanDescriptorSet::addBufferToDescriptor(uint32_t binding, uint32_t bufferSize, void const* data)
{
	assert(buffers.count(binding) != 0U && "Buffer binding not existent in descriptor set");

	std::vector<VkWriteDescriptorSet> writes(framesInFlight);
	

	for (uint32_t i = 0; i < framesInFlight; i++)
	{
		buffers[binding][i] = std::make_unique<VulkanBuffer>(
			vulkanDevice,
			bufferSize,
			1,
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
		);
		buffers[binding][i]->map();
		buffers[binding][i]->writeToBuffer(data);

		VkDescriptorBufferInfo bufferInfo = buffers[binding][i]->descriptorInfo();

		writes[i].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writes[i].dstBinding = binding;
		writes[i].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		writes[i].descriptorCount = 1;
		writes[i].dstSet = descriptorSets[i];
		writes[i].pBufferInfo = &bufferInfo;
	}

	vkUpdateDescriptorSets(vulkanDevice.device(), writes.size(), writes.data(), 0, nullptr);
}

void VulkanDescriptorSet::addSamplerToDescriptor(uint32_t binding, const std::string& texturePath, TextureType type)
{
	assert(textures.count(binding) != 0U && "Sampler binding not existent in descriptor set");

	textures[binding] = std::make_unique<VulkanTexture>(vulkanDevice, texturePath, type);
	std::vector<VkWriteDescriptorSet> writes(framesInFlight);

	for (uint32_t i = 0; i < framesInFlight; i++)
	{
		VkDescriptorImageInfo imageInfo = textures[binding]->getDescriptorImageInfo();

		writes[i].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writes[i].dstBinding = binding;
		writes[i].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		writes[i].descriptorCount = 1;
		writes[i].dstSet = descriptorSets[i];
		writes[i].pImageInfo = &imageInfo;
	}

	vkUpdateDescriptorSets(vulkanDevice.device(), writes.size(), writes.data(), 0, nullptr);
}

}	// namespace ve
