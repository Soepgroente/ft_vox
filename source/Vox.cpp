#include "Vox.hpp"
#include "Stopwatch.hpp"
#include "Stopwatch.hpp"
#include "Utils.hpp"

#include <iostream>
#include <chrono>
#include <cassert>

namespace vox {

std::vector<std::thread> Vox::workerThreads{};

/**
 * Create the engine of the game
 */
Vox::Vox( void ) :
	vulkanWindow{Config::defaultWindowHeight, Config::defaultWindowWidth, "Vox"},
	vulkanDevice{vulkanWindow},
	vulkanRenderer{vulkanWindow, vulkanDevice},
	vulkanSetFactory{vulkanDevice},
	camera{vec3{165.0f, 225.0f, 165.0f}, ve::CameraSettings::cameraForward, Config::cameraLimitsMov},
	voxelMap{threadManager},
	inputHandler{
		[this](vec2 const& cursorPos) { this->rotateCameraFromCursorPos(cursorPos); },
		[this](i32 width, i32 height) { this->resizeWindow(width, height); }

	},
	updateMatrixUbo{false}
{
	Vox::workerThreads.reserve(std::max(std::thread::hardware_concurrency() - 1, 0U));

	this->camera.setViewMatrix();
	this->camera.setPerspectiveProjection(
		radians(ve::CameraSettings::projectionFov),
		this->vulkanWindow.getAspectRatio(),
		ve::CameraSettings::projectionNear,
		ve::CameraSettings::projectionFar
	);
	voxelMap.init();
	assert(voxelMap.isReady() == true && "map wasn't ready");
	this->inputHandler.setCallbacks(vulkanWindow.getGLFWwindow());
}

void Vox::setupVulkan( void )
{
	uint32_t	maxSetsToCreate = 2;
	uint32_t	nUniformDescriptors = 2;
	uint32_t	nSamplerDescriptors = 2;

	this->vulkanSetFactory
		.setMaxSets(maxSetsToCreate)
		.setFramesInFlight(ve::VulkanSwapChain::MAX_FRAMES_IN_FLIGHT)
		.addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, nUniformDescriptors)
		.addPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, nSamplerDescriptors)
		.createPool();

	ve::VulkanBindingSet terrainSetBindings;
	terrainSetBindings.addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT);

	ve::VulkanBindingSet skyboxSetBindings;
	skyboxSetBindings.addBinding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
	skyboxSetBindings.addBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);

	this->matrixDescriptorSet = this->vulkanSetFactory.createDescriptorSet(terrainSetBindings);
	MatrixUBO ubo(this->camera);
	this->matrixDescriptorSet->addBufferToDescriptor(0, sizeof(ubo), static_cast<void*>(&ubo));

	this->samplersDescriptorSet = this->vulkanSetFactory.createDescriptorSet(skyboxSetBindings);
	this->samplersDescriptorSet->addSamplerToDescriptor(0, Config::texture2VoxelPath, ve::TextureType::TEXTURE_PLAIN);
	this->samplersDescriptorSet->addSamplerToDescriptor(1, Config::textureSkyboxPath, ve::TextureType::TEXTURE_CUBEMAP);

	this->terrainModel = this->voxelMap.createNewModel(vulkanDevice);
	this->skyBoxModel = this->createSkyboxModel();

	std::vector<VkDescriptorSetLayout> descriptorSetLayouts{this->matrixDescriptorSet->getDescriptorSetLayout(), this->samplersDescriptorSet->getDescriptorSetLayout()};
	this->terrainPipeline = ve::VulkanPipeline::createPipeline(
		this->vulkanDevice,
		descriptorSetLayouts,
		this->vulkanRenderer.getSwapChainRenderPass(),
		Config::terrainVertShaderPath,
		Config::terrainFragShaderPath,
		*this->terrainModel,
		false
	);

	this->skyboxPipeline = ve::VulkanPipeline::createPipeline(
		this->vulkanDevice,
		descriptorSetLayouts,
		this->vulkanRenderer.getSwapChainRenderPass(),
		Config::skyboxVertShaderPath,
		Config::skyboxFragShaderPath,
		*this->skyBoxModel,
		true
	);
}

/**
 * Run the rendering loop
 */
void Vox::run( void )
{
	Stopwatch timer;

	std::cout << "\n\n\n\n";
	while (vulkanWindow.shouldClose() == false)
	{
		glfwPollEvents();
		timer.start();

		this->moveCamera(timer.elapsed(Unit::Seconds));

		vec3 playerPos = this->camera.getCameraPos();
		if (voxelMap.update(playerPos) == true)
		{
			this->terrainModel = voxelMap.createNewModel(vulkanDevice);
		}

		VkCommandBuffer commandBuffer = this->vulkanRenderer.beginFrame();
		if (commandBuffer != nullptr)
		{
			this->vulkanRenderer.beginSwapChainRenderPass(commandBuffer);
			
			ui32 currentFrame = this->vulkanRenderer.getCurrentFrameIndex();
			this->matrixDescriptorSet->setCurrentFrame(currentFrame);
			this->samplersDescriptorSet->setCurrentFrame(currentFrame);

			if (this->updateMatrixUbo == true)
			{
				MatrixUBO ubo(this->camera);
				this->matrixDescriptorSet->updateUbo(0, ubo.getData());
				this->updateMatrixUbo = false;
			}

			this->matrixDescriptorSet->bind(commandBuffer, *this->terrainPipeline, 0U);
			this->samplersDescriptorSet->bind(commandBuffer, *this->skyboxPipeline, 1U);

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

		// std::cout << "\033[K" << "Player position - x: " << playerPos.x << " y: " << playerPos.y << " z: " << playerPos.z << std::endl;
		// int	fps = static_cast<int> (1.0f / timer.elapsed(Unit::Seconds));
		// std::cout << "\033[3A" << "\033[K" << "Frames per second: " << fps << ", Frame time: " << timer.elapsed(Unit::Milliseconds) << "ms " << std::endl;
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
void Vox::moveCamera( float deltaTime )
{
	if (this->inputHandler.isKeyPressed(GLFW_KEY_W))
	{
		this->camera.moveForward(deltaTime * Config::movementSpeed);
		this->updateMatrixUbo = true;
	}

	if (this->inputHandler.isKeyPressed(GLFW_KEY_A))
	{
		this->camera.moveLeft(deltaTime * Config::movementSpeed);
		this->updateMatrixUbo = true;
	}

	if (this->inputHandler.isKeyPressed(GLFW_KEY_S))
	{
		this->camera.moveBackward(deltaTime * Config::movementSpeed);
		this->updateMatrixUbo = true;
	}

	if (this->inputHandler.isKeyPressed(GLFW_KEY_D))
	{
		this->camera.moveRight(deltaTime * Config::movementSpeed);
		this->updateMatrixUbo = true;
	}

	if (this->inputHandler.isKeyPressed(GLFW_KEY_E))
	{
		this->camera.moveUp(deltaTime * Config::movementSpeed);
		this->updateMatrixUbo = true;
	}

	if (this->inputHandler.isKeyPressed(GLFW_KEY_Q))
	{
		this->camera.moveDown(deltaTime * Config::movementSpeed);
		this->updateMatrixUbo = true;
	}

	if (this->inputHandler.isKeyPressed(GLFW_KEY_UP))
	{
		this->camera.rotate(deltaTime * Config::lookSpeed, 0.0f, 0.0f);
		this->updateMatrixUbo = true;
	}

	if (this->inputHandler.isKeyPressed(GLFW_KEY_DOWN))
	{
		this->camera.rotate(-deltaTime * Config::lookSpeed, 0.0f, 0.0f);
		this->updateMatrixUbo = true;
	}

	if (this->inputHandler.isKeyPressed(GLFW_KEY_LEFT))
	{
		this->camera.rotate(0.0f, -deltaTime * Config::lookSpeed, 0.0f);
		this->updateMatrixUbo = true;
	}

	if (this->inputHandler.isKeyPressed(GLFW_KEY_RIGHT))
	{
		this->camera.rotate(0.0f, deltaTime * Config::lookSpeed, 0.0f);
		this->updateMatrixUbo = true;
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
void	Vox::rotateCameraFromCursorPos( vec2 const& currPos )
{
	vec2 const& oldPos = this->inputHandler.getCursorPos();

	float yaw = (currPos.x - oldPos.x) * ve::CameraSettings::cameraSensitivity;
	float pitch = (oldPos.y - currPos.y) * ve::CameraSettings::cameraSensitivity;  // reversed since y-coordinates range from bottom to top
	this->camera.rotate(pitch, yaw, 0.0f);
	this->updateMatrixUbo = true;
}

/**
 * When a resize of the window happens, updates Vulkan and recalcolate projection matrix 
 * (since ration w/h changed)
 *
 * @param width new width
 *
 * @param height new height
 */
void Vox::resizeWindow( ui32 width, ui32 height )
{
	this->vulkanWindow.resetWindowSize(width, height);
	this->camera.setPerspectiveProjection(
		radians(ve::CameraSettings::projectionFov),
		this->vulkanWindow.getAspectRatio(),
		ve::CameraSettings::projectionNear,
		ve::CameraSettings::projectionFar
	);
	this->updateMatrixUbo = true;
}

/**
 * Creates a new ve::VulkanModel, that loads vertex data into the GPU. It shall be called everytime
 * a new world/chunks is created (i.e. whenever WorldNavigator::spawnCloseByWorlds() returns true)
 *
 * @param device vulkan object used to build the buffers
 *
 * @return pointer to the newly created model
 */
std::unique_ptr<ve::VulkanModel> Vox::createSkyboxModel( void )
{
	return std::make_unique<ve::VulkanModel>(vulkanDevice, getVertexRelative(vec3{-0.5f, -0.5f, -0.5f}), getIndexRelative());
}

}	// namespace vox
