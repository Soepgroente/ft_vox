#include "Vox.hpp"
#include "Stopwatch.hpp"
#include "Utils.hpp"

#include <chrono>


namespace vox {

std::vector<std::thread> Vox::workerThreads{};

struct TerrainUBO
{
	mat4				model{1.0f};
	mat4				view{1.0f};
	mat4				projection{1.0f};
	vec4				ambientLightColor{1.0f, 1.0f, 1.0f, 0.1f};
	vec3				lightPosition{0.0f, -4.0f, -3.0f};
	alignas(16)	vec4	lightColor{1.0f};
};

struct SkyboxUBO
{
	mat4				view{1.0f};
	mat4				projection{1.0f};
};

/**
 * Create the engine of the game
 */
Vox::Vox( void ) :
	vulkanWindow{Config::defaultWindowHeight, Config::defaultWindowWidth, "Vox"},
	vulkanDevice{vulkanWindow},
	vulkanRenderer{vulkanWindow, vulkanDevice},
	camera{Config::startingPos, ve::CameraSettings::cameraForward, Config::cameraLimitsMov},
	navigator{Config::worldSize},
	inputHandler(
		[this](vec2 const& cursorPos) { this->rotateCameraFromCursorPos(cursorPos); },
		[this](int32_t width, int32_t height) { this->resizeWindow(width, height); }
	)
{
	Vox::workerThreads.reserve(std::max(std::thread::hardware_concurrency() - 1, 0U));

	this->camera.setViewMatrix();
	this->camera.setPerspectiveProjection(
		radians(ve::CameraSettings::projectionFov),
		this->vulkanWindow.getAspectRatio(),
		ve::CameraSettings::projectionNear,
		ve::CameraSettings::projectionFar
	);
	this->inputHandler.setCallbacks(vulkanWindow.getGLFWwindow());

	this->textures.insert({TEXT_DIRT_1, ve::VulkanTexture{Config::texture2VoxelPath, vulkanDevice, ve::TextureType::TEXTURE_PLAIN}});
	this->textures.insert({TEXT_SKYBOX, ve::VulkanTexture{Config::textureSkyboxPath, vulkanDevice, ve::TextureType::TEXTURE_CUBEMAP}});
}

/**
 * Run the rendering loop
 */
void Vox::run( void ) {
	std::vector<std::unique_ptr<ve::VulkanBuffer>>	terrainUboBuffers(ve::VulkanSwapChain::MAX_FRAMES_IN_FLIGHT);
	for (size_t i = 0; i < terrainUboBuffers.size(); i++)
	{
		terrainUboBuffers[i] = std::make_unique<ve::VulkanBuffer>(
			vulkanDevice,
			sizeof(TerrainUBO),
			1,
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
		);
		terrainUboBuffers[i]->map();
	}

	std::vector<std::unique_ptr<ve::VulkanBuffer>>	skyboxUboBuffers(ve::VulkanSwapChain::MAX_FRAMES_IN_FLIGHT);
	for (size_t i = 0; i < skyboxUboBuffers.size(); i++)
	{
		skyboxUboBuffers[i] = std::make_unique<ve::VulkanBuffer>(
			vulkanDevice,
			sizeof(SkyboxUBO),
			1,
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
		);
		skyboxUboBuffers[i]->map();
	}

	ui32 maxDescriptors = 2;
	ui32 nUboBuffers = 2;
	ui32 nTextures = 2;
	std::unique_ptr<ve::VulkanDescriptorPool> globalDescriptorPool = ve::VulkanDescriptorPool::Builder(vulkanDevice)
		.setMaxSets(maxDescriptors * ve::VulkanSwapChain::MAX_FRAMES_IN_FLIGHT)
		.addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, nUboBuffers * ve::VulkanSwapChain::MAX_FRAMES_IN_FLIGHT)
		.addPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, nTextures * ve::VulkanSwapChain::MAX_FRAMES_IN_FLIGHT)
		.build();

	std::unique_ptr<ve::VulkanDescriptorSetLayout> globalSetLayout = ve::VulkanDescriptorSetLayout::Builder(vulkanDevice)
		.addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT)
		.addBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
		.build();

	std::vector<VkDescriptorSet> terrainDescriptorSets(ve::VulkanSwapChain::MAX_FRAMES_IN_FLIGHT);
	for (size_t i = 0; i < terrainDescriptorSets.size(); i++)
	{
		VkDescriptorBufferInfo bufferInfo = terrainUboBuffers[i]->descriptorInfo();
		VkDescriptorImageInfo imageInfo{};

		imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		imageInfo.imageView = this->textures.at(TEXT_DIRT_1).getImageView();
		imageInfo.sampler = this->textures.at(TEXT_DIRT_1).getSampler();

		ve::VulkanDescriptorWriter(*globalSetLayout, *globalDescriptorPool)
			.writeBuffer(0, &bufferInfo)
			.writeImage(1, &imageInfo)
			.build(terrainDescriptorSets[i]);
	}

	std::vector<VkDescriptorSet> skyboxDescriptorSets(ve::VulkanSwapChain::MAX_FRAMES_IN_FLIGHT);
	for (size_t i = 0; i < skyboxDescriptorSets.size(); i++)
	{
		VkDescriptorBufferInfo bufferInfo = skyboxUboBuffers[i]->descriptorInfo();
		VkDescriptorImageInfo imageInfo{};

		imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		imageInfo.imageView = this->textures.at(TEXT_SKYBOX).getImageView();
		imageInfo.sampler = this->textures.at(TEXT_SKYBOX).getSampler();

		ve::VulkanDescriptorWriter(*globalSetLayout, *globalDescriptorPool)
			.writeBuffer(0, &bufferInfo)
			.writeImage(1, &imageInfo)
			.build(skyboxDescriptorSets[i]);
	}

	ve::VulkanRenderSystem	terrainRenderSystem{
		vulkanDevice,
		vulkanRenderer.getSwapChainRenderPass(),
		std::vector<VkDescriptorSetLayout>{globalSetLayout->getDescriptorSetLayout()},
		Config::terrainVertShaderPath,
		Config::terrainFragShaderPath,
		ve::ModelType::VERTEX | ve::ModelType::NORMAL | ve::ModelType::TEXTURE,
		ve::TextureType::TEXTURE_PLAIN
	};

	ve::VulkanRenderSystem	skyboxRenderSystem{
		vulkanDevice,
		vulkanRenderer.getSwapChainRenderPass(),
		std::vector<VkDescriptorSetLayout>{globalSetLayout->getDescriptorSetLayout()},
		Config::skyboxVertShaderPath,
		Config::skyboxFragShaderPath,
		ve::ModelType::VERTEX,
		ve::TextureType::TEXTURE_CUBEMAP
	};

	ve::FrameInfo terrainRenderingInfo{
		0,
		this->camera,
		nullptr,
		nullptr,
		ve::VulkanObject::createVulkanObject(),
	};

	ve::FrameInfo skyboxRenderingInfo{
		0,
		this->camera,
		nullptr,
		nullptr,
		ve::VulkanObject::createVulkanObject(),
	};

	this->navigator.spawnCloseByWorlds(this->camera.getCameraPos());
	// this->navigator.spawnCloseByWorlds(this->camera.getCameraPos(), this->threadManager);
	terrainRenderingInfo.gameObject.model = this->navigator.createNewModel(vulkanDevice);
	skyboxRenderingInfo.gameObject.model = this->createSkyboxModel();

	Stopwatch timer;
	std::cout << "\n\n\n\n";
	while (vulkanWindow.shouldClose() == false)
	{
		glfwPollEvents();
		timer.start();
		// do game operations
		this->moveCamera(timer.elapsed(Seconds));
		// add chunks of maps if necessary
		if (this->navigator.borderCrossed(this->camera.getCameraPos()) == true) {
			bool newDataCreated = this->navigator.spawnCloseByWorlds(this->camera.getCameraPos(), this->threadManager);
			// bool newDataCreated = this->navigator.spawnCloseByWorlds(this->camera.getCameraPos());
			if (newDataCreated)
				terrainRenderingInfo.gameObject.model = this->navigator.createNewModel(vulkanDevice);
		}
		VkCommandBuffer commandBuffer = vulkanRenderer.beginFrame();
		if (commandBuffer != nullptr)
		{
			terrainRenderingInfo.commandBuffer = commandBuffer;
			skyboxRenderingInfo.commandBuffer = commandBuffer;
			vulkanRenderer.beginSwapChainRenderPass(commandBuffer);
			terrainRenderingInfo.frameIndex = vulkanRenderer.getCurrentFrameIndex();

			TerrainUBO terrainUbo{};
			terrainUbo.model = mat4::idMat();
			terrainUbo.view = this->camera.getViewMatrix();
			terrainUbo.projection = this->camera.getProjectionMatrix();
			terrainUboBuffers[terrainRenderingInfo.frameIndex]->writeToBuffer(&terrainUbo);
			terrainUboBuffers[terrainRenderingInfo.frameIndex]->flush();
			terrainRenderingInfo.globalDescriptorSet = terrainDescriptorSets[terrainRenderingInfo.frameIndex];
			terrainRenderSystem.renderObject(terrainRenderingInfo);

			SkyboxUBO skyboxUbo{};
			skyboxUbo.view = this->camera.getViewMatrixOnlyRotation();
			skyboxUbo.projection = this->camera.getProjectionMatrix();
			skyboxRenderingInfo.frameIndex = terrainRenderingInfo.frameIndex;
			skyboxUboBuffers[skyboxRenderingInfo.frameIndex]->writeToBuffer(&skyboxUbo);
			skyboxUboBuffers[skyboxRenderingInfo.frameIndex]->flush();
			skyboxRenderingInfo.globalDescriptorSet = skyboxDescriptorSets[skyboxRenderingInfo.frameIndex];
			skyboxRenderSystem.renderObject(skyboxRenderingInfo);

			vulkanRenderer.endSwapChainRenderPass(commandBuffer);
			vulkanRenderer.endFrame();
			timer.stop();
			int	fps = static_cast<int> (1.0f / timer.elapsed(Seconds));
			std::cout << "\033[3A" << "\033[K" << "Frames per second: " << fps << ", Frame time: " << timer.elapsed(Milliseconds) << "ms " << std::endl;
			vec3 playerPos = this->camera.getCameraPos();
			std::cout << "\033[K" << "Player position - x: " << playerPos.x << " y: " << playerPos.y << " z: " << playerPos.z << std::endl;
			std::cout << "\033[K" << "GPU memory used: " << formatBytes(this->navigator.getMemoryUsed()) << std::endl;
		}
		this->inputHandler.reset();
		// frameCount++;
	}

	vkDeviceWaitIdle(vulkanDevice.device());
}

/**
 * Handle camera transformation in case of keys W-A-S-D or up-left-bottom-right (arrow) keys are pressed
 *
 * @param deltaTime to normalize the transformation, so that it doesn't depend on the fps
 * 
 * @note camera rotation using a key will be removed in the final version
 */
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

/**
 * Handle camera rotation my cursor movement
 *
 * @param newX x position (relative to the monitor) of the cursor ( (0;0): top-left corner)
 *
 * @param newY y position (relative to the monitor) of the cursor ( (0;0): top-left corner)
 *
 * @return a vector of 36 uin32_t starting from the offset value
 */
void Vox::rotateCameraFromCursorPos( vec2 const& currPos ) {
	vec2 const& oldPos = this->inputHandler.getCursorPos();

	float yaw = (currPos.x - oldPos.x) * ve::CameraSettings::cameraSensitivity;
	float pitch = (oldPos.y - currPos.y) * ve::CameraSettings::cameraSensitivity;  // reversed since y-coordinates range from bottom to top
	this->camera.rotate(pitch, yaw, 0.0f);
}

/**
 * When a resize of the window happens, updates Vulkan and recalcolate projection matrix 
 * (since ration w/h changed)
 *
 * @param width new width
 *
 * @param height new height
 */
void Vox::resizeWindow( ui32 width, ui32 height ) {
	this->vulkanWindow.resetWindowSize(width, height);
	this->camera.setPerspectiveProjection(
		radians(ve::CameraSettings::projectionFov),
		this->vulkanWindow.getAspectRatio(),
		ve::CameraSettings::projectionNear,
		ve::CameraSettings::projectionFar
	);
}

/**
 * Creates a new ve::VulkanModel, that loads vertex data into the GPU. It shall be called everytime
 * a new world/chunks is created (i.e. whenever WorldNavigator::spawnCloseByWorlds() returns true)
 *
 * @param device vulkan object used to build the buffers
 *
 * @return pointer to the newly created model
 */
std::unique_ptr<ve::VulkanModel> Vox::createSkyboxModel( void ) {
	// return std::make_unique<ve::VulkanModel>(vulkanDevice, getVertexRelative(), getIndexRelative());
	return std::make_unique<ve::VulkanModel>(vulkanDevice, getVertexRelative(vec3{-0.5f, -0.5f, -0.5f}), getIndexRelative());
}

}	// namespace vox
