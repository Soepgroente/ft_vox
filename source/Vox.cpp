#include "Vox.hpp"

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

Vox::Vox( void ) : 
	objModelPath("models/teapot.obj"),
	camera(vec3(0.0f), vec3{0.0f, ve::CameraSettings::cameraDistance, 0.0f}),
	world(vec3ui{Config::worldSize, Config::worldSize, Config::worldSize}, true)
{
	globalDescriptorPool = ve::VulkanDescriptorPool::Builder(vulkanDevice)
		.setMaxSets(ve::VulkanSwapChain::MAX_FRAMES_IN_FLIGHT)
		.addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, ve::VulkanSwapChain::MAX_FRAMES_IN_FLIGHT)
		.addPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, ve::VulkanSwapChain::MAX_FRAMES_IN_FLIGHT)
		.build();
	inputHandler.setCallbacks(vulkanWindow.getGLFWwindow());

	createObjects();
}

Vox::~Vox( void )
{
	globalDescriptorPool.reset();
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

	float aspectRatio = this->vulkanWindow.getAspectRatio();
	camera.setViewMatrix();
	camera.setPerspectiveProjection(
		radians(ve::CameraSettings::projectionFov),
		aspectRatio,
		ve::CameraSettings::projectionNear,
		ve::CameraSettings::projectionFar
	);

	float	elapsedTime = 0.0f;
	std::chrono::high_resolution_clock::time_point	currentTime, newTime;
	VkCommandBuffer		commandBuffer = nullptr;
	size_t				frameCount = 0;
	currentTime = std::chrono::high_resolution_clock::now();

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

	vec3	playerPos(.0f);
	while (vulkanWindow.shouldClose() == false)
	{
		glfwPollEvents();
		newTime = std::chrono::high_resolution_clock::now();
		elapsedTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
		currentTime = newTime;
		this->moveCamera(elapsedTime);
		this->rotateCamera();

		commandBuffer = vulkanRenderer.beginFrame();

		if (commandBuffer != nullptr)
		{
			int frameIndex = vulkanRenderer.getCurrentFrameIndex();
			GlobalUBO	ubo{};

			info.frameIndex = frameIndex;
			info.commandBuffer = commandBuffer;
			info.globalDescriptorSet = globalDescriptorSets[frameIndex];
			ubo.projectionView = camera.getProjectionMatrix() * camera.getViewMatrix();
			uboBuffers[frameIndex]->writeToBuffer(&ubo);
			uboBuffers[frameIndex]->flush();

			vulkanRenderer.beginSwapChainRenderPass(commandBuffer);
			renderSystem.renderObjects(info);

			newTime = std::chrono::high_resolution_clock::now();
			std::cout << "\033[2A" << "\033[K" << "Frames per second: " << static_cast<int> (1.0f / elapsedTime) << ", Frame time: ";
			std::cout << std::chrono::duration<float, std::chrono::milliseconds::period>(newTime - currentTime).count() << "ms "<< std::endl;

			playerPos = info.camera.getCameraPos();
			std::cout << "\033[K" << "Player position - x: " << playerPos.x << " y: " << playerPos.y << " z: " << playerPos.z << std::endl;

			vulkanRenderer.endSwapChainRenderPass(commandBuffer);
			vulkanRenderer.endFrame();
		}
		this->inputHandler.reset();
		frameCount++;
	}
	vkDeviceWaitIdle(vulkanDevice.device());
}

void Vox::createObjects( void )
{
	world.createNewWorld(VoxelGrid::voxelGenerator8);

	ve::VulkanModel::Builder			builder = world.generateBufferData(false);
	std::shared_ptr<ve::VulkanModel>	model = ve::VulkanModel::createModel(vulkanDevice, builder);
	ve::VulkanObject 					object = ve::VulkanObject::createVulkanObject();

	object.model = std::move(model);
	object.color = {1.0f, 0.4f, 0.2f};
	object.transform.translation = object.model->getBoundingCenter().inverted();

	objects.emplace(object.getID(), std::move(object));
	textures.reserve(5);
	for (size_t i = 0; i < 5; i++)
	{
		ve::VulkanTexture texture("textures/derp" + std::to_string(i + 1) + ".jpeg", vulkanDevice);
		textures.emplace_back(std::move(texture));
	}
}

void Vox::shutdown( void )
{
}

InputHandler const& Vox::getHandler( void ) const noexcept {
	return this->inputHandler;
}

InputHandler& Vox::getHandler( void ) noexcept {
	return this->inputHandler;
}

void Vox::moveCamera( float deltaTime ) {
	if (this->inputHandler.isKeyPressed(GLFW_KEY_W))
		this->camera.moveForward(deltaTime * Config::movementSpeed);

	if (this->inputHandler.isKeyPressed(GLFW_KEY_A))
		this->camera.moveLeft(deltaTime * Config::movementSpeed);

	if (this->inputHandler.isKeyPressed(GLFW_KEY_S))
		this->camera.moveBackward(deltaTime * Config::movementSpeed);

	if (this->inputHandler.isKeyPressed(GLFW_KEY_D))
		this->camera.moveRight(deltaTime * Config::movementSpeed);

	if (this->inputHandler.isKeyPressed(GLFW_KEY_E))
		this->camera.moveUp(deltaTime * Config::movementSpeed);

	if (this->inputHandler.isKeyPressed(GLFW_KEY_Q))
		this->camera.moveDown(deltaTime * Config::movementSpeed);

	if (this->inputHandler.isKeyPressed(GLFW_KEY_UP))
		this->camera.rotate(deltaTime * Config::lookSpeed, 0.0f, 0.0f);

	if (this->inputHandler.isKeyPressed(GLFW_KEY_DOWN))
		this->camera.rotate(-deltaTime * Config::lookSpeed, 0.0f, 0.0f);

	if (this->inputHandler.isKeyPressed(GLFW_KEY_LEFT))
		this->camera.rotate(0.0f, -deltaTime * Config::lookSpeed, 0.0f);

	if (this->inputHandler.isKeyPressed(GLFW_KEY_RIGHT))
		this->camera.rotate(0.0f, deltaTime * Config::lookSpeed, 0.0f);
}

void Vox::rotateCamera( void ) {
	float deltaX, deltaY;
	if (this->inputHandler.cursorPositionHasChanged(deltaX, deltaY) == false)
		return;

	float yaw = deltaX * ve::CameraSettings::cameraSensitivity;
	float pitch = -deltaY * ve::CameraSettings::cameraSensitivity;  // reversed since y-coordinates range from bottom to top

	if (pitch > 89.0f)
		pitch =  89.0f;
	else if(pitch < -89.0f)
		pitch = -89.0f;
	this->camera.rotate(pitch, yaw, 0.0f);
}

}	// namespace vox
