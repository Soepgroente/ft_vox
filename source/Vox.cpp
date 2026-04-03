#include "Vox.hpp"
#include "Stopwatch.hpp"
#include "Utils.hpp"

#include <iostream>
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
	vulkanSetFactory{vulkanDevice, ve::VulkanSwapChain::MAX_FRAMES_IN_FLIGHT},
	camera{Config::startingPos, ve::CameraSettings::cameraForward, Config::cameraLimitsMov},
	navigator{Config::worldSize},
	inputHandler{
		[this](vec2 const& cursorPos) { this->rotateCameraFromCursorPos(cursorPos); },
		[this](i32 width, i32 height) { this->resizeWindow(width, height); }

	},
	playerMoved{false}
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
}

void Vox::setupVulkan( void ) {
	this->vulkanSetFactory
		.setMaxSets(2)
		.addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 2)
		.addPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 2)
		.createPool()
		.addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT, 1);

	this->matrixDescriptorSet = this->vulkanSetFactory.createDescriptorSet();

	MatrixUBO ubo(this->camera);
	this->matrixDescriptorSet->addBufferToDescriptor(0, ubo.getSize(), ubo.getData());

	this->vulkanSetFactory
		.resetBindings()
		.addBinding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 1)
		.addBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 1);

	this->samplersDescriptorSet = this->vulkanSetFactory.createDescriptorSet();
	this->samplersDescriptorSet->addSamplerToDescriptor(0, Config::texture1VoxelPath, ve::TextureType::TEXTURE_PLAIN);
	this->samplersDescriptorSet->addSamplerToDescriptor(1, Config::textureSkyboxPath, ve::TextureType::TEXTURE_CUBEMAP);

	std::vector<VkDescriptorSetLayout> descriptorSets = std::vector<VkDescriptorSetLayout>{this->matrixDescriptorSet->getDescriptorSetLayout(), this->samplersDescriptorSet->getDescriptorSetLayout()};
	terrainPipeline = ve::VulkanPipeline::createPipeline(
		this->vulkanDevice,
		descriptorSets,
		Config::terrainVertShaderPath,
		Config::terrainFragShaderPath,
		this->vulkanRenderer.getSwapChainRenderPass(),
		ve::ModelType::VERTEX | ve::ModelType::NORMAL | ve::ModelType::TEXTURE,
		false
	);

	skyboxPipeline = ve::VulkanPipeline::createPipeline(
		this->vulkanDevice,
		descriptorSets,
		Config::skyboxVertShaderPath,
		Config::skyboxFragShaderPath,
		this->vulkanRenderer.getSwapChainRenderPass(),
		ve::ModelType::VERTEX,
		true
	);
}

/**
 * Run the rendering loop
 */
void Vox::run( void ) {
	this->navigator.spawnCloseByWorlds(this->camera.getCameraPos());
	// this->navigator.spawnCloseByWorlds(this->camera.getCameraPos(), this->threadManager);
	std::unique_ptr<ve::VulkanModel> terrainModel = this->navigator.createNewModel(vulkanDevice);
	std::unique_ptr<ve::VulkanModel> skyBoxModel = this->createSkyboxModel();

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
				terrainModel = this->navigator.createNewModel(vulkanDevice);
		}

		VkCommandBuffer commandBuffer = this->vulkanRenderer.beginFrame();
		if (commandBuffer != nullptr)
		{
			uint32_t currentFrame = this->vulkanRenderer.getCurrentFrameIndex();
			this->vulkanRenderer.beginSwapChainRenderPass(commandBuffer);

			this->matrixDescriptorSet->setCurrentFrame(currentFrame);
			this->samplersDescriptorSet->setCurrentFrame(currentFrame);
			
			if (this->playerMoved == true) {
				MatrixUBO ubo(this->camera);
				this->matrixDescriptorSet->updateUbo(0, ubo.getData());
				this->playerMoved = false;
			}
			this->matrixDescriptorSet->bind(commandBuffer, *this->terrainPipeline);
			this->samplersDescriptorSet->bind(commandBuffer, *this->skyboxPipeline);

			this->terrainPipeline->bind(commandBuffer);
			terrainModel->bind(commandBuffer);
			terrainModel->draw(commandBuffer);

			this->skyboxPipeline->bind(commandBuffer);
			skyBoxModel->bind(commandBuffer);
			skyBoxModel->draw(commandBuffer);

			this->vulkanRenderer.endSwapChainRenderPass(commandBuffer);
			this->vulkanRenderer.endFrame();
		}
		this->inputHandler.reset();
		timer.stop();

		int	fps = static_cast<int> (1.0f / timer.elapsed(Seconds));
		std::cout << "\033[3A" << "\033[K" << "Frames per second: " << fps << ", Frame time: " << timer.elapsed(Milliseconds) << "ms " << std::endl;
		vec3 playerPos = this->camera.getCameraPos();
		std::cout << "\033[K" << "Player position - x: " << playerPos.x << " y: " << playerPos.y << " z: " << playerPos.z << std::endl;
		std::cout << "\033[K" << "GPU memory used: " << formatBytes(this->navigator.getMemoryUsed()) << std::endl;
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
	if (this->inputHandler.isKeyPressed(GLFW_KEY_W)) {
		this->camera.moveForward(deltaTime * Config::movementSpeed);
		this->playerMoved = true;
	}

	if (this->inputHandler.isKeyPressed(GLFW_KEY_A)) {
		this->camera.moveLeft(deltaTime * Config::movementSpeed);
		this->playerMoved = true;
	}

	if (this->inputHandler.isKeyPressed(GLFW_KEY_S)) {
		this->camera.moveBackward(deltaTime * Config::movementSpeed);
		this->playerMoved = true;
	}

	if (this->inputHandler.isKeyPressed(GLFW_KEY_D)) {
		this->camera.moveRight(deltaTime * Config::movementSpeed);
		this->playerMoved = true;
	}

	if (this->inputHandler.isKeyPressed(GLFW_KEY_E)) {
		this->camera.moveUp(deltaTime * Config::movementSpeed);
		this->playerMoved = true;
	}

	if (this->inputHandler.isKeyPressed(GLFW_KEY_Q)) {
		this->camera.moveDown(deltaTime * Config::movementSpeed);
		this->playerMoved = true;
	}

	if (this->inputHandler.isKeyPressed(GLFW_KEY_UP)) {
		this->camera.rotate(deltaTime * Config::lookSpeed, 0.0f, 0.0f);
		this->playerMoved = true;
	}

	if (this->inputHandler.isKeyPressed(GLFW_KEY_DOWN)) {
		this->camera.rotate(-deltaTime * Config::lookSpeed, 0.0f, 0.0f);
		this->playerMoved = true;
	}

	if (this->inputHandler.isKeyPressed(GLFW_KEY_LEFT)) {
		this->camera.rotate(0.0f, -deltaTime * Config::lookSpeed, 0.0f);
		this->playerMoved = true;
	}

	if (this->inputHandler.isKeyPressed(GLFW_KEY_RIGHT)) {
		this->camera.rotate(0.0f, deltaTime * Config::lookSpeed, 0.0f);
		this->playerMoved = true;
	}
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
	this->playerMoved = true;
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
	this->playerMoved = true;
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
