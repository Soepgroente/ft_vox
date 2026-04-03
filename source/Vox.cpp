#include "Vox.hpp"
#include "Stopwatch.hpp"
#include "Stopwatch.hpp"
#include "Utils.hpp"

#include <chrono>


namespace vox {

struct GlobalUBO
{
	mat4				projectionView{1.0f};
	vec4				ambientLightColor{1.0f, 1.0f, 1.0f, 0.1f};
	vec3				lightPosition{0.0f, -4.0f, -3.0f};
	alignas(16)	vec4	lightColor{1.0f};
};

/**
 * Create the engine of the game
 */
Vox::Vox( void ) :
	vulkanWindow{Config::defaultWindowHeight, Config::defaultWindowWidth, "Vox"},
	vulkanDevice{vulkanWindow},
	vulkanRenderer{vulkanWindow, vulkanDevice},
	globalDescriptorPool{},
	camera{vec3::zero(), ve::CameraSettings::cameraForward, Config::cameraLimitsMov},
	voxelMap{threadManager},
	inputHandler(
		[this](vec2 const& cursorPos) { this->rotateCameraFromCursorPos(cursorPos); },
		[this](int32_t width, int32_t height) { this->resizeWindow(width, height); }
	)
{
	globalDescriptorPool = ve::VulkanDescriptorPool::Builder(vulkanDevice)
		.setMaxSets(ve::VulkanSwapChain::MAX_FRAMES_IN_FLIGHT)
		.addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, ve::VulkanSwapChain::MAX_FRAMES_IN_FLIGHT)
		.addPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, ve::VulkanSwapChain::MAX_FRAMES_IN_FLIGHT)
		.build();

	this->camera.setViewMatrix();
	this->camera.setPerspectiveProjection(
		radians(ve::CameraSettings::projectionFov),
		this->vulkanWindow.getAspectRatio(),
		ve::CameraSettings::projectionNear,
		ve::CameraSettings::projectionFar
	);
	this->inputHandler.setCallbacks(vulkanWindow.getGLFWwindow());
	voxelMap.init();
	vec3 mapMiddle = voxelMap.getMapMiddle();
	this->camera.moveBackward(mapMiddle.z);
	this->camera.moveLeft(mapMiddle.x);
	this->camera.moveUp(mapMiddle.y);
}

/**
 * destructor
 */
Vox::~Vox( void ) noexcept {
	globalDescriptorPool.reset();
}

/**
 * Run the rendering loop
 *
 * @param start the starting value of the face indexes
 *
 * @return a vector of 36 uin32_t starting from the offset value
 */

static std::vector<std::unique_ptr<ve::VulkanBuffer>>	initUboBuffers(ve::VulkanDevice& device)
{
	std::vector<std::unique_ptr<ve::VulkanBuffer>>	uboBuffers(ve::VulkanSwapChain::MAX_FRAMES_IN_FLIGHT);

	for (size_t i = 0; i < uboBuffers.size(); i++)
	{
		uboBuffers[i] = std::make_unique<ve::VulkanBuffer>(
			device,
			sizeof(GlobalUBO),
			1,
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
		);
		uboBuffers[i]->map();
	}
	return uboBuffers;
}

void	Vox::run( void ) {
	std::vector<std::unique_ptr<ve::VulkanBuffer>>	uboBuffers = initUboBuffers(vulkanDevice);
	ve::VulkanTexture texture{Config::texture2VoxelPath, vulkanDevice};

	std::unique_ptr<ve::VulkanDescriptorSetLayout> globalSetLayout = ve::VulkanDescriptorSetLayout::Builder(vulkanDevice)
		.addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS)
		.addBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
		.build();
	std::vector<VkDescriptorSet>	globalDescriptorSets(ve::VulkanSwapChain::MAX_FRAMES_IN_FLIGHT);

	for (size_t i = 0; i < globalDescriptorSets.size(); i++)
	{
		VkDescriptorBufferInfo bufferInfo = uboBuffers[i]->descriptorInfo();
		VkDescriptorImageInfo imageInfo{};

		imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		imageInfo.imageView = texture.getImageView();
		imageInfo.sampler = texture.getSampler();

		ve::VulkanDescriptorWriter(*globalSetLayout, *globalDescriptorPool)
			.writeBuffer(0, &bufferInfo)
			.writeImage(1, &imageInfo)
			.build(globalDescriptorSets[i]);
	}
	ve::VulkanRenderSystem	renderSystem{
		vulkanDevice,
		vulkanRenderer.getSwapChainRenderPass(),
		globalSetLayout->getDescriptorSetLayout(),
		Config::vertShaderPath,
		Config::fragShaderPath
	};

	// size_t	frameCount = 0;
	Stopwatch timer;

	ve::FrameInfo info
	{
		0,
		this->camera,
		nullptr,
		nullptr,
		ve::VulkanObject::createVulkanObject(),
	};
	info.gameObject.model = voxelMap.createNewModel(vulkanDevice);
	// this->navigator.spawnCloseByWorlds(this->camera.getCameraPos());
	// this->navigator.spawnCloseByWorlds(this->camera.getCameraPos(), this->threadManager);
	// info.gameObject.model = this->navigator.createNewModel(vulkanDevice);

	std::cout << "\n\n\n\n";
	while (vulkanWindow.shouldClose() == false)
	{
		glfwPollEvents();
		this->moveCamera(timer.elapsed(Unit::Seconds));
		vec3 playerPos = info.camera.getCameraPos();
		timer.start();

		if (voxelMap.update(playerPos) == true)
		{
			info.gameObject.model = voxelMap.createNewModel(vulkanDevice);
		}

		info.commandBuffer = vulkanRenderer.beginFrame();
		if (info.commandBuffer != nullptr)
		{
			int frameIndex = vulkanRenderer.getCurrentFrameIndex();
			GlobalUBO	ubo{};
			info.frameIndex = frameIndex;
			info.globalDescriptorSet = globalDescriptorSets[frameIndex];
			ubo.projectionView = this->camera.getProjectionMatrix() * this->camera.getViewMatrix();
			uboBuffers[frameIndex]->writeToBuffer(&ubo);
			uboBuffers[frameIndex]->flush();

			vulkanRenderer.beginSwapChainRenderPass(info.commandBuffer);
			renderSystem.renderObject(info);

			// std::cout << "\033[K" << "Player position - x: " << playerPos.x << " y: " << playerPos.y << " z: " << playerPos.z << std::endl;
			// std::cout << "\033[K" << "GPU memory used: " << formatBytes(this->navigator.getMemoryUsed()) << std::endl;

			vulkanRenderer.endSwapChainRenderPass(info.commandBuffer);
			vulkanRenderer.endFrame();
			timer.stop();
			// int	fps = static_cast<int> (1.0f / timer.elapsed(Unit::Seconds));
			// std::cout << "\033[3A" << "\033[K" << "Frames per second: " << fps << ", Frame time: " << timer.elapsed(Unit::Milliseconds) << "ms " << std::endl;
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
void	Vox::moveCamera( float deltaTime ) {
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
void	Vox::rotateCameraFromCursorPos( vec2 const& currPos ) {
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
void	Vox::resizeWindow( uint32_t width, uint32_t height ) {
	this->vulkanWindow.resetWindowSize(width, height);
	this->camera.setPerspectiveProjection(
		radians(ve::CameraSettings::projectionFov),
		this->vulkanWindow.getAspectRatio(),
		ve::CameraSettings::projectionNear,
		ve::CameraSettings::projectionFar
	);
}

}	// namespace vox
