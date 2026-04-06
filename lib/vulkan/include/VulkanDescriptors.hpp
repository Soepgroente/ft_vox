#pragma once

#include "VulkanDevice.hpp"
#include "VulkanBuffer.hpp"
#include "VulkanTexture.hpp"
#include "VulkanPipeline.hpp"

#include <memory>
#include <unordered_map>
#include <vector>
#include <map>


namespace ve {

// class VulkanDescriptorSetLayout
// {
// 	public:
//
// 	class Builder
// 	{
// 		public:
//
// 		Builder(VulkanDevice& vulkanDevice) : vulkanDevice{vulkanDevice} {}
//
// 		Builder&	addBinding(
// 			uint32_t binding,
// 			VkDescriptorType descriptorType,
// 			VkShaderStageFlags stageFlags,
// 			uint32_t count = 1
// 		);
// 		std::unique_ptr<VulkanDescriptorSetLayout> build() const;
//
// 		private:
//
// 		VulkanDevice&	vulkanDevice;
// 		std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings{};
// 	};
//
// 	VulkanDescriptorSetLayout(VulkanDevice& vulkanDevice, std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings);
// 	~VulkanDescriptorSetLayout();
// 	VulkanDescriptorSetLayout(const VulkanDescriptorSetLayout&) = delete;
// 	VulkanDescriptorSetLayout&	operator=(const VulkanDescriptorSetLayout&) = delete;
//
// 	VkDescriptorSetLayout getDescriptorSetLayout() const { return descriptorSetLayout; }
//
// 	private:
//
// 	VulkanDevice&			vulkanDevice;
// 	VkDescriptorSetLayout	descriptorSetLayout;
//
// 	std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings;
//
// 	friend class VulkanDescriptorWriter;
// };
//
// class VulkanDescriptorPool
// {
// 	public:
//
// 	class Builder
// 	{
// 		public:
//
// 		Builder(VulkanDevice& vulkanDevice) : vulkanDevice{vulkanDevice} {}
//
// 		Builder&	addPoolSize(VkDescriptorType descriptorType, uint32_t count);
// 		Builder&	setPoolFlags(VkDescriptorPoolCreateFlags flags);
// 		Builder&	setMaxSets(uint32_t count);
// 		std::unique_ptr<VulkanDescriptorPool>	build() const;
//
// 		private:
//
// 		VulkanDevice&		vulkanDevice;
// 		std::vector<VkDescriptorPoolSize>	poolSizes{};
// 		uint32_t	maxSets = 1000;
// 		VkDescriptorPoolCreateFlags	poolFlags = 0;
// 	};
//
// 	VulkanDescriptorPool(
// 		VulkanDevice& vulkanDevice,
// 		uint32_t maxSets,
// 		VkDescriptorPoolCreateFlags poolFlags,
// 		const std::vector<VkDescriptorPoolSize>& poolSizes
// 	);
// 	~VulkanDescriptorPool();
// 	VulkanDescriptorPool(const VulkanDescriptorPool&) = delete;
// 	VulkanDescriptorPool&	operator=(const VulkanDescriptorPool&) = delete;
//
// 	bool	allocateDescriptor(const VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet& descriptor) const;
// 	void	freeDescriptors(std::vector<VkDescriptorSet>& descriptors) const;
// 	void	resetPool();
//
// 	private:
//
// 	VulkanDevice&		vulkanDevice;
// 	VkDescriptorPool	descriptorPool;
//
// 	friend class VulkanDescriptorWriter;
// };
//
// class VulkanDescriptorWriter
// {
// 	public:
//
// 	VulkanDescriptorWriter(VulkanDescriptorSetLayout& setLayout, VulkanDescriptorPool& pool, int32_t framesInFlight);
//
// 	VulkanDescriptorWriter&	writeBuffer(uint32_t binding, VkDescriptorBufferInfo *bufferInfo);
// 	VulkanDescriptorWriter&	writeImage(uint32_t binding, VkDescriptorImageInfo *imageInfo);
//
// 	bool	build(VkDescriptorSet& set);
// 	void	overwrite(VkDescriptorSet& set);
//
// 	private:
//
// 	VulkanDescriptorSetLayout&	setLayout;
// 	VulkanDescriptorPool&		pool;
// 	std::vector<VkWriteDescriptorSet> writes;
// 	int32_t						framesInFlight;
//
// };
//
// class VulkanDescriptorBinding
// {
// 	public:
//
// 	// VulkanDescriptorBinding() = default;
// 	// ~VulkanDescriptorBinding() = default;
// 	// VulkanDescriptorBinding(const VulkanDescriptorBinding&) = default;
// 	// VulkanDescriptorBinding(VulkanDescriptorBinding&&) = default;
// 	// VulkanDescriptorBinding& operator=(const VulkanDescriptorBinding&) = default;
// 	// VulkanDescriptorBinding& operator=(VulkanDescriptorBinding&&) = default;
//
// 	// void*												getBindingData(uint32_t binding) const {return bindingData.at(binding); };
// 	const std::vector<VkDescriptorSetLayoutBinding>&	getBindingInfo() const noexcept {return bindingInfo; };
// 	// size_t												getNbindings() const noexcept {return bindingInfo.size(); };
//
// 	VulkanDescriptorBinding&	addBinding(uint32_t binding, VkDescriptorType descriptorType, VkShaderStageFlags stageFlags);
// 	VulkanDescriptorBinding&	resetBindings() noexcept;
//
// 	private:
//
// 	// std::map<uint32_t,VkDescriptorSetLayoutBinding>	bindings1;
// 	std::vector<VkDescriptorSetLayoutBinding>		bindingInfo;
// 	// std::map<uint32_t,void*>						bindingData;
// };


class VulkanBindingSet {
	public:
		VulkanBindingSet&									addBinding( uint32_t binding, VkDescriptorType type, VkShaderStageFlags stage, uint32_t count = 1U );
		std::vector<VkDescriptorSetLayoutBinding> const&	getbindings( void ) const noexcept { return this->bindings; };

	private:
		std::vector<VkDescriptorSetLayoutBinding> bindings;
};

class VulkanDescriptorSet;

class VulkanDescriptorSetFactory
{
	public:
		VulkanDescriptorSetFactory( void ) = delete;
		VulkanDescriptorSetFactory( VulkanDevice& vulkanDevice) : vulkanDevice{vulkanDevice} {};
		~VulkanDescriptorSetFactory( void );
		VulkanDescriptorSetFactory( VulkanDescriptorSetFactory const& other) = delete;
		VulkanDescriptorSetFactory& operator=( VulkanDescriptorSetFactory const& other ) = delete;

		VulkanDescriptorSetFactory&	addPoolSize( VkDescriptorType type, uint32_t count );
		VulkanDescriptorSetFactory&	setPoolFlags( VkDescriptorPoolCreateFlags flags ) noexcept;
		VulkanDescriptorSetFactory&	setMaxSets( uint32_t count ) noexcept;
		VulkanDescriptorSetFactory&	setFramesInFlight( uint32_t framesInFlight ) noexcept;
		VulkanDescriptorSetFactory& createPool( void );
		VulkanDescriptorSetFactory&	resetPool( void ) noexcept;

		std::unique_ptr<VulkanDescriptorSet>	createDescriptorSet( VulkanBindingSet const& bindings ) const;

	private:
		VulkanDevice&						vulkanDevice;
		uint32_t							framesInFlight = 1U;
		VkDescriptorPoolCreateFlags			poolFlags = 0U;
		uint32_t							maxSets = 100U;

		std::vector<VkDescriptorPoolSize>	poolSizes;
		VkDescriptorPool					descriptorPool{};
};

class VulkanDescriptorSet
{
	public:
		VulkanDescriptorSet( void ) = delete;
		VulkanDescriptorSet(
			VulkanDevice& 			vulkanDevice,
			VkDescriptorSetLayout	descriptorSetLayout,
			uint32_t				framesInFlight,
			VkDescriptorPool		descriptorPool,
			VulkanBindingSet const&	bindings
		);
		~VulkanDescriptorSet( void );
		VulkanDescriptorSet(const VulkanDescriptorSet&) = delete;
		VulkanDescriptorSet& operator=(const VulkanDescriptorSet&) = delete;

		void	setCurrentFrame(uint32_t frame) noexcept;
		void	updateUbo(int32_t binding, void const* data);
		void	bind(VkCommandBuffer commandBuffer, const VulkanPipeline& pipeline, uint32_t setIndex);

		VkDescriptorSetLayout	getDescriptorSetLayout( void ) const noexcept { return descriptorSetLayout; };
		void					addBufferToDescriptor(uint32_t binding, uint32_t bufferSize, void const* data);
		void					addSamplerToDescriptor(uint32_t binding, const std::string& texturePath, TextureType type);

	private:
		VulkanDevice&					vulkanDevice;
		VkDescriptorSetLayout			descriptorSetLayout;
		uint32_t						framesInFlight = 1U;
		uint32_t						currentFrame = 0U;

		std::vector<VkDescriptorSet>									descriptorSets;
		std::map<int32_t,std::vector<std::unique_ptr<VulkanBuffer>>>	buffers;
		std::map<int32_t,std::unique_ptr<VulkanTexture>>				textures;

		friend class VulkanDescriptorSetFactory;
};



}  // namespace ve
