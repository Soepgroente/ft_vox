#include "Vox.hpp"
#include "Camera.hpp"
#include "InputHandler.hpp"
#include "Vulkan.hpp"

#include <chrono>
#include <random>

namespace vox {

struct GlobalUBO
{
	mat4				projectionView{1.0f};
	vec4				ambientLightColor{1.0f, 1.0f, 1.0f, 0.1f};
	vec3				lightPosition{0.0f, -4.0f, -3.0f};
	alignas(16)	vec4	lightColor{1.0f};
};

Vox::Vox( std::string const& objPath ) : objModelPath(objPath)
{
	globalDescriptorPool = ve::VulkanDescriptorPool::Builder(vulkanDevice)
		.setMaxSets(ve::VulkanSwapChain::MAX_FRAMES_IN_FLIGHT)
		.addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, ve::VulkanSwapChain::MAX_FRAMES_IN_FLIGHT)
		.addPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, ve::VulkanSwapChain:: MAX_FRAMES_IN_FLIGHT)
		.build();
	loadObjects();
	if (objects.size() > 1)
	{
		std::cerr << "More than one object is out of... Scop... for this project. Get it? Scop. 🥁" << std::endl;
		std::exit(EXIT_FAILURE);
	}
}

Vox::~Vox( void )
{
	globalDescriptorPool.reset();
}


bool Vox::initialize( void )
{
    return true;
}

void Vox::run( void )
{
    std::vector<std::unique_ptr<ve::VulkanBuffer>>	uboBuffers(ve::VulkanSwapChain::MAX_FRAMES_IN_FLIGHT);

	for (size_t i = 0; i < uboBuffers.size(); i++)
	{
		uboBuffers[i] = std::make_unique<ve::VulkanBuffer>(
			vulkanDevice,
			sizeof(GlobalUBO),
			1,
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
		);
		uboBuffers[i]->map();
	}

	std::unique_ptr<ve::VulkanDescriptorSetLayout> globalSetLayout = ve::VulkanDescriptorSetLayout::Builder(vulkanDevice)
		.addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS)
		.addBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
		.build();
	std::vector<VkDescriptorSet>	globalDescriptorSets(ve::VulkanSwapChain::MAX_FRAMES_IN_FLIGHT);
	ve::VulkanTexture texture{"textures/derpy_cats.jpg", vulkanDevice};
	// ve::VulkanTexture texture{"textures/viking_room.png", vulkanDevice};

	for (size_t i = 0; i < globalDescriptorSets.size(); i++)
	{
		VkDescriptorBufferInfo bufferInfo = uboBuffers[i]->descriptorInfo();
		VkDescriptorImageInfo imageInfo{};

		imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		imageInfo.imageView = texture.getImageView();
		imageInfo.sampler = texture. getSampler();

		ve::VulkanDescriptorWriter(*globalSetLayout, *globalDescriptorPool)
			.writeBuffer(0, &bufferInfo)
			.writeImage(1, &imageInfo)
			.build(globalDescriptorSets[i]);
	}

	ve::VulkanRenderSystem	renderSystem{vulkanDevice, vulkanRenderer.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout()};
	ve::Camera camera{};

	camera.setViewTarget(vec3{0.0f}, vec3{0.0f});
	
	float	elapsedTime = 0.0f;
	float	aspectRatio = 1.0f;
	std::chrono::high_resolution_clock::time_point	currentTime, newTime;
	VkCommandBuffer	commandBuffer = nullptr;
	ve::VulkanObject	viewerObject = ve::VulkanObject::createVulkanObject();
	ve::KeyboardInput	keyboardInput(vulkanWindow.getGLFWwindow());
	ve::MouseInput		mouseInput(vulkanWindow.getGLFWwindow());
	size_t			frameCount = 0;

	currentTime = std::chrono::high_resolution_clock::now();
	viewerObject.transform.translation = {0.0f, 0.0f, -10.0f};
	ve::FrameInfo	info
	{
		0,
		elapsedTime,
		camera,
		commandBuffer,
		nullptr,
		objects,
		false,
		false
	};

	while (vulkanWindow.shouldClose() == false)
	{
		glfwPollEvents();
		newTime = std::chrono::high_resolution_clock::now();
		elapsedTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
		currentTime = newTime;
		keyboardInput.move(viewerObject, elapsedTime);
		mouseInput.move(viewerObject, elapsedTime);

		camera.setViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);
		aspectRatio = vulkanRenderer.getAspectRatio();
		camera.setPerspectiveProjection(radians(50.0f), aspectRatio, 0.1f, 1000.0f);
		commandBuffer = vulkanRenderer.beginFrame();

		if (commandBuffer != nullptr)
		{
			int frameIndex = vulkanRenderer.getCurrentFrameIndex();
			GlobalUBO	ubo{};

			info.frameIndex = frameIndex;
			info.commandBuffer = commandBuffer;
			info.globalDescriptorSet = globalDescriptorSets[frameIndex];
			info.useTexture = keyboardInput.shouldShowTextures(frameCount, info.useTexture);
			info.rotateModel = keyboardInput.shouldRotate(frameCount, info.rotateModel);
			ubo.projectionView = camera.getProjectionMatrix() * camera.getViewMatrix();
			uboBuffers[frameIndex]->writeToBuffer(&ubo);
			uboBuffers[frameIndex]->flush();

			vulkanRenderer.beginSwapChainRenderPass(commandBuffer);
			renderSystem.renderObjects(info);

			newTime = std::chrono::high_resolution_clock::now();
			std::cout << "\rFrames per second: " << static_cast<int> (1.0f / elapsedTime) << ", Frame time: ";
			std::cout << std::chrono::duration<float, std::chrono::milliseconds::period>(newTime - currentTime).count() << "ms " << std::flush;		

			vulkanRenderer.endSwapChainRenderPass(commandBuffer);
			vulkanRenderer.endFrame();
		}
		frameCount++;
	}
	vkDeviceWaitIdle(vulkanDevice.device());
}

void Vox::loadObjects()
{
	std::shared_ptr<ve::VulkanModel>	model = ve::VulkanModel::createModelFromFile(vulkanDevice, objModelPath);
	ve::VulkanObject					object = ve::VulkanObject::createVulkanObject();

	object.model = std::move(model);
	object.color = {1.0f, 0.4f, 0.2f};
	object.transform.translation = object.model->getBoundingCenter().inverted();
	objects.emplace(object.getID(), std::move(object));
	textures.reserve(5);
	for (size_t i = 0; i < 5; i++)
	{
		ve::VulkanTexture	texture("textures/derp" + std::to_string(i + 1) + ".jpeg", vulkanDevice);

		textures.emplace_back(std::move(texture));
	}
}

void Vox::shutdown( void )
{

}

}	// namespace vox
