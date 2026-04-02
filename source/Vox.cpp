#include "Vox.hpp"
#include "Stopwatch.hpp"
#include "Utils.hpp"

#include <chrono>


namespace vox {

std::vector<std::thread> Vox::workerThreads{};

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
		[this](i32 width, i32 height) { this->resizeWindow(width, height); }
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
	this->terrainUboBuffers.reserve(ve::VulkanSwapChain::MAX_FRAMES_IN_FLIGHT);
	this->skyboxUboBuffers.reserve(ve::VulkanSwapChain::MAX_FRAMES_IN_FLIGHT);
}

void Vox::setupVulkan( void ) {
	// for (size_t i = 0; i < terrainUboBuffers.size(); i++)
	// {
	// 	terrainUboBuffers[i] = std::make_unique<ve::VulkanBuffer>(
	// 		vulkanDevice,
	// 		sizeof(TerrainUBO),
	// 		1,
	// 		VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
	// 		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
	// 	);
	// 	terrainUboBuffers[i]->map();
	// }

	// for (size_t i = 0; i < skyboxUboBuffers.size(); i++)
	// {
	// 	skyboxUboBuffers[i] = std::make_unique<ve::VulkanBuffer>(
	// 		vulkanDevice,
	// 		sizeof(SkyboxUBO),
	// 		1,
	// 		VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
	// 		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
	// 	);
	// 	skyboxUboBuffers[i]->map();
	// }

	ui32 maxDescriptors = 2;
	ui32 nUboBuffers = 2;
	ui32 nTextures = 2;

	ve::VulkanDescriptorSetFactory descSetFactory(this->vulkanDevice, ve::VulkanSwapChain::MAX_FRAMES_IN_FLIGHT);
	descSetFactory
		.setMaxSets(maxDescriptors)
		.addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, nUboBuffers)
		.addPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, nTextures)
		.createPool();

	// both desc. sets have same layout (1 ubo 1 sampler) so one is enough to spawn both different sets
	std::vector<ve::VulkanBindingEntry> bindingsLayout(2);
	bindingsLayout[0].binding = 0;
	bindingsLayout[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	bindingsLayout[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	bindingsLayout[0].count = 1;
	bindingsLayout[0].bufferInfo = &terrainUboBuffers[0]->descriptorInfo();
	bindingsLayout[1].binding = 1;
	bindingsLayout[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	bindingsLayout[1].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	bindingsLayout[1].count = 1;
	bindingsLayout[1].imageInfo = &this->textures.at(TEXT_DIRT_1).getDescriptorImageInfo();
	this->terrainDescriptorSet = descSetFactory.createSet(bindingsLayout);

	bindingsLayout[0].bufferInfo = &skyboxUboBuffers[0]->descriptorInfo();
	bindingsLayout[1].imageInfo = &this->textures.at(TEXT_SKYBOX).getDescriptorImageInfo();
	this->skyboxDescriptorSet = descSetFactory.createSet(bindingsLayout);

	terrainPipeline = std::make_unique<ve::VulkanRenderSystem>(
		vulkanDevice,
		this->vulkanRenderer.getSwapChainRenderPass(),
		std::vector<VkDescriptorSetLayout>{descriptorSetLayout->getDescriptorSetLayout()},
		Config::terrainVertShaderPath,
		Config::terrainFragShaderPath,
		ve::ModelType::VERTEX | ve::ModelType::NORMAL | ve::ModelType::TEXTURE,
		ve::TextureType::TEXTURE_PLAIN
	);

	skyboxPipeline = std::make_unique<ve::VulkanRenderSystem>(
		vulkanDevice,
		this->vulkanRenderer.getSwapChainRenderPass(),
		std::vector<VkDescriptorSetLayout>{descriptorSetLayout->getDescriptorSetLayout()},
		Config::skyboxVertShaderPath,
		Config::skyboxFragShaderPath,
		ve::ModelType::VERTEX,
		ve::TextureType::TEXTURE_CUBEMAP
	);
}

/**
 * Run the rendering loop
 */
void Vox::run( void ) {
	
	ve::FrameInfo terrainRenderingInfo{
		0,
		// this->camera,
		nullptr,
		nullptr,
		ve::VulkanObject::createVulkanObject(),
	};

	ve::FrameInfo skyboxRenderingInfo{
		0,
		// this->camera,
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

	TerrainUBO terrainUbo;
	terrainUbo.model = mat4::idMat();
	terrainUbo.view = this->camera.getViewMatrix();
	terrainUbo.projection = this->camera.getProjectionMatrix();

	SkyboxUBO skyboxUbo;
	skyboxUbo.view = this->camera.getViewMatrixOnlyRotation();
	skyboxUbo.projection = this->camera.getProjectionMatrix();

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
		VkCommandBuffer commandBuffer = this->vulkanRenderer.beginFrame();
		if (commandBuffer != nullptr)
		{
			this->vulkanRenderer.beginSwapChainRenderPass(commandBuffer);

			// terrainRenderingInfo.frameIndex = this->vulkanRenderer.getCurrentFrameIndex();
			// terrainRenderingInfo.globalDescriptorSet = terrainDescriptorSets[terrainRenderingInfo.frameIndex];
			// terrainRenderingInfo.commandBuffer = commandBuffer;
			terrainUbo.view = this->camera.getViewMatrix();
			this->terrainUboBuffers[terrainRenderingInfo.frameIndex]->writeToBuffer(&terrainUbo);
			terrainRenderingInfo.globalDescriptorSet = terrainDescriptorSets[terrainRenderingInfo.frameIndex];
			this->terrainPipeline->renderObject(terrainRenderingInfo);

			// skyboxRenderingInfo.frameIndex = this->vulkanRenderer.getCurrentFrameIndex();
			// skyboxRenderingInfo.globalDescriptorSet = skyboxDescriptorSets[skyboxRenderingInfo.frameIndex];
			// skyboxRenderingInfo.commandBuffer = commandBuffer;
			skyboxUbo.view = this->camera.getViewMatrixOnlyRotation();
			skyboxRenderingInfo.frameIndex = terrainRenderingInfo.frameIndex;
			this->skyboxUboBuffers[skyboxRenderingInfo.frameIndex]->writeToBuffer(&skyboxUbo);
			skyboxRenderingInfo.globalDescriptorSet = skyboxDescriptorSets[skyboxRenderingInfo.frameIndex];
			this->skyboxPipeline->renderObject(skyboxRenderingInfo);

			this->vulkanRenderer.endSwapChainRenderPass(commandBuffer);
			this->vulkanRenderer.endFrame();
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
