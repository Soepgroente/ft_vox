#include "Vox.hpp"
#include "Stopwatch.hpp"
#include "Stopwatch.hpp"
#include "Utils.hpp"

#include <iostream>
#include <chrono>
#include <cassert>

namespace vox {

/**
 * Create the engine of the game
 */
Vox::Vox( void ) :
	vulkanWindow{Config::defaultWindowHeight, Config::defaultWindowWidth, "Vox"},
	vulkanDevice{vulkanWindow},
	vulkanRenderer{vulkanWindow, vulkanDevice},
	vulkanSetFactory{vulkanDevice},
	camera{Config::cameraStartPos, Config::cameraForward, this->vulkanWindow.getAspectRatio()},
	voxelMap{threadManager},
	inputHandler{
		[this](vec2 const& cursorPos) { this->rotateCameraFromCursorPos(cursorPos); },
		[this](i32 width, i32 height) { this->resizeWindow(width, height); }
	},
	updateUniforms{false}
{
	this->voxelMap.init();
	assert(this->voxelMap.isReady() == true && "map wasn't ready");
	this->inputHandler.setCallbacks(this->vulkanWindow.getGLFWwindow());
}

void Vox::setupVulkan( void )
{
	uint32_t	maxSetsToCreate = 5;
	uint32_t	nUniformDescriptors = 2;
	uint32_t	nSamplerDescriptors = 4;

	this->terrainObject = std::make_unique<ve::VulkanObject>();
	this->undergroundObject = std::make_unique<ve::VulkanObject>();
	this->skyboxObject = std::make_unique<ve::VulkanObject>();

	this->vulkanSetFactory
		.setMaxSets(maxSetsToCreate)
		.setFramesInFlight(ve::VulkanSwapChain::MAX_FRAMES_IN_FLIGHT)
		.addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, nUniformDescriptors)
		.addPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, nSamplerDescriptors)
		.createPool();

	ve::VulkanBindingSet uboSetBindings;
	uboSetBindings.addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT);
	uboSetBindings.addBinding(1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT);

	this->uboDescriptorSet = this->vulkanSetFactory.createDescriptorSet(uboSetBindings);
	this->uboDescriptorSet->addBufferDescriptor(0, sizeof(ViewProjectUBO));
	this->uboDescriptorSet->addBufferDescriptor(1, sizeof(LightUBO));

	ve::VulkanBindingSet textureTerrainSetBindings;
	textureTerrainSetBindings.addBinding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
	this->textTerrainDescriptorSet = this->vulkanSetFactory.createDescriptorSet(textureTerrainSetBindings);
	this->textTerrainDescriptorSet->addSamplerDescriptor(0, Config::textureDirtPath, ve::TextureType::TEXTURE_PLAIN);

	ve::VulkanBindingSet textureUndergroundSetBindings;
	textureUndergroundSetBindings.addBinding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
	this->textUndergroundDescriptorSet = this->vulkanSetFactory.createDescriptorSet(textureUndergroundSetBindings);
	this->textUndergroundDescriptorSet->addSamplerDescriptor(0, Config::textureStonePath, ve::TextureType::TEXTURE_PLAIN);

	ve::VulkanBindingSet textureSkyboxSetBindings;
	textureSkyboxSetBindings.addBinding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
	this->textSkyboxDescriptorSet = this->vulkanSetFactory.createDescriptorSet(textureSkyboxSetBindings);
	this->textSkyboxDescriptorSet->addSamplerDescriptor(0, Config::textureSkyboxPath, ve::TextureType::TEXTURE_CUBEMAP);

	this->terrainObject->setModel(this->voxelMap.createNewModelTerrain(vulkanDevice));
	this->undergroundObject->setModel(this->voxelMap.createNewModelUnderground(vulkanDevice));
	this->skyboxObject->setModel(this->createSkyboxModel());

	std::vector<VkDescriptorSetLayout> descriptorSetLayouts{
		this->uboDescriptorSet->getDescriptorSetLayout(),
		this->textTerrainDescriptorSet->getDescriptorSetLayout(),
		this->textUndergroundDescriptorSet->getDescriptorSetLayout(),
		this->textSkyboxDescriptorSet->getDescriptorSetLayout()
	};
	this->terrainPipeline = ve::VulkanPipeline::createPipeline(
		this->vulkanDevice,
		descriptorSetLayouts,
		this->vulkanRenderer.getSwapChainRenderPass(),
		Config::lightVertShaderPath,
		Config::lightFragShaderPath,
		this->terrainObject->getVboLayout(),
		false,
		sizeof(MeshData)
	);

	this->skyboxPipeline = ve::VulkanPipeline::createPipeline(
		this->vulkanDevice,
		descriptorSetLayouts,
		this->vulkanRenderer.getSwapChainRenderPass(),
		Config::skyboxVertShaderPath,
		Config::skyboxFragShaderPath,
		this->skyboxObject->getVboLayout(),
		true
	);
}

/**
 * Run the rendering loop
 */
void Vox::run( void )
{
	ViewProjectUBO	matrixUbo(this->camera.getViewMatrix(), this->camera.getProjectionMatrix());
	LightUBO		lightUbo(Config::sunPos, Config::lightColor, this->camera.getCameraPos());
	MeshData		terrainData{mat4::idMat(), mat4::idMat()};
	terrainData.updateMaterial(Config::dirtMaterial);

	this->uboDescriptorSet->updateUboAll(0, matrixUbo.getData());
	this->uboDescriptorSet->updateUboAll(1, lightUbo.getData());

	float deltaTime = 0.0f;
	Stopwatch timer;
	std::cout << "\n\n\n\n";
	while (vulkanWindow.shouldClose() == false)
	{
		glfwPollEvents();
		timer.start();
		deltaTime = timer.elapsed(Unit::Seconds);			// NB why is it right after start?
		this->moveCamera(deltaTime);

		// vec3 playerPos = this->camera.getCameraPos();
		// if (voxelMap.update(playerPos) == true)
		// {
		// 	this->terrainObject = voxelMap.createNewModelTerrain(vulkanDevice);
		// }

		VkCommandBuffer commandBuffer = this->vulkanRenderer.beginFrame();
		if (commandBuffer != nullptr)
		{
			this->vulkanRenderer.beginSwapChainRenderPass(commandBuffer);
			
			ui32 currentFrame = this->vulkanRenderer.getCurrentFrameIndex();
			this->uboDescriptorSet->setCurrentFrame(currentFrame);
			this->textTerrainDescriptorSet->setCurrentFrame(currentFrame);
			this->textUndergroundDescriptorSet->setCurrentFrame(currentFrame);
			this->textSkyboxDescriptorSet->setCurrentFrame(currentFrame);

			if (this->updateUniforms == true)
			{
				matrixUbo.updateView(this->camera.getViewMatrix());
				matrixUbo.updateProjection(this->camera.getProjectionMatrix());
				this->uboDescriptorSet->updateUboAll(0, matrixUbo.getData());
				lightUbo.updateViewPos(this->camera.getCameraPos());
				this->uboDescriptorSet->updateUboAll(1, lightUbo.getData());
				this->updateUniforms = false;
			}

			this->terrainPipeline->bindPipeline(commandBuffer);
			this->uboDescriptorSet->bindSet(commandBuffer, *this->terrainPipeline, 0U);
			this->textTerrainDescriptorSet->bindSet(commandBuffer, *this->terrainPipeline, 1U);
			this->terrainPipeline->updatePushConstants(commandBuffer, terrainData.getData());

			this->terrainObject->bindBuffer(commandBuffer);
			this->terrainObject->draw(commandBuffer);

			this->textUndergroundDescriptorSet->bindSet(commandBuffer, *this->terrainPipeline, 1U);

			this->undergroundObject->bindBuffer(commandBuffer);
			this->undergroundObject->draw(commandBuffer);

			this->skyboxPipeline->bindPipeline(commandBuffer);
			this->uboDescriptorSet->bindSet(commandBuffer, *this->skyboxPipeline, 0U);
			this->textSkyboxDescriptorSet->bindSet(commandBuffer, *this->skyboxPipeline, 1U);

			this->skyboxObject->bindBuffer(commandBuffer);
			this->skyboxObject->draw(commandBuffer);

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
		this->updateUniforms = true;
	}
	if (this->inputHandler.isKeyPressed(GLFW_KEY_A))
	{
		this->camera.moveLeft(deltaTime * Config::movementSpeed);
		this->updateUniforms = true;
	}
	if (this->inputHandler.isKeyPressed(GLFW_KEY_S))
	{
		this->camera.moveBackward(deltaTime * Config::movementSpeed);
		this->updateUniforms = true;
	}
	if (this->inputHandler.isKeyPressed(GLFW_KEY_D))
	{
		this->camera.moveRight(deltaTime * Config::movementSpeed);
		this->updateUniforms = true;
	}
	if (this->inputHandler.isKeyPressed(GLFW_KEY_E))
	{
		this->camera.moveUp(deltaTime * Config::movementSpeed);
		this->updateUniforms = true;
	}
	if (this->inputHandler.isKeyPressed(GLFW_KEY_Q))
	{
		this->camera.moveDown(deltaTime * Config::movementSpeed);
		this->updateUniforms = true;
	}
	if (this->inputHandler.isKeyPressed(GLFW_KEY_UP))
	{
		this->camera.rotate(deltaTime * Config::lookSpeed, 0.0f, 0.0f);
		this->updateUniforms = true;
	}
	if (this->inputHandler.isKeyPressed(GLFW_KEY_DOWN))
	{
		this->camera.rotate(-deltaTime * Config::lookSpeed, 0.0f, 0.0f);
		this->updateUniforms = true;
	}
	if (this->inputHandler.isKeyPressed(GLFW_KEY_LEFT))
	{
		this->camera.rotate(0.0f, -deltaTime * Config::lookSpeed, 0.0f);
		this->updateUniforms = true;
	}
	if (this->inputHandler.isKeyPressed(GLFW_KEY_RIGHT))
	{
		this->camera.rotate(0.0f, deltaTime * Config::lookSpeed, 0.0f);
		this->updateUniforms = true;
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

	float yaw = (currPos.x - oldPos.x) * CameraSettings::cameraSensitivity;
	float pitch = (oldPos.y - currPos.y) * CameraSettings::cameraSensitivity;  // reversed since y-coordinates range from bottom to top
	this->camera.rotate(pitch, yaw, 0.0f);
	this->updateUniforms = true;
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
	this->camera.updateAspect(this->vulkanWindow.getAspectRatio());
	this->updateUniforms = true;
}

/**
 * Creates a new ve::VulkanModel, that loads vertex data into the GPU. It shall be called everytime
 * a new world/chunks is created (i.e. whenever WorldNavigator::spawnCloseByWorlds() returns true)
 *
 * @param device vulkan object used to build the buffers
 *
 * @return pointer to the newly created model
 */
std::shared_ptr<ve::VulkanModel> Vox::createSkyboxModel( void )
{
	return std::make_shared<ve::VulkanModel>(vulkanDevice, getVertexRelative(vec3{-0.5f, -0.5f, -0.5f}), getIndexRelative());
}

}	// namespace vox
