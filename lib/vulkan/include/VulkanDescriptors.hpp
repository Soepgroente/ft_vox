#pragma once

#include "VulkanDevice.hpp"
#include "VulkanPipeline.hpp"

#include <memory>
#include <vector>
#include <map>


namespace ve {

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
		VulkanDescriptorSet( VulkanDescriptorSet const& other ) = delete;
		VulkanDescriptorSet( VulkanDescriptorSet&& other );
		VulkanDescriptorSet& operator=( VulkanDescriptorSet const& other ) = delete;

		void	setCurrentFrame( uint32_t frame ) noexcept;
		void	updateUbo( int32_t binding, void const* data );
		void	bind( VkCommandBuffer commandBuffer, const VulkanPipeline& pipeline, uint32_t setIndex );

		VkDescriptorSetLayout	getDescriptorSetLayout( void ) const noexcept { return descriptorSetLayout; };
		void					addBufferToDescriptor( uint32_t binding, uint32_t bufferSize, void const* data );
		void					addSamplerToDescriptor( uint32_t binding, const std::string& texturePath, TextureType type );

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
