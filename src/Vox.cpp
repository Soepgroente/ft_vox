#include "Vox.hpp"

#include <chrono>
#include <random>

namespace vox {

void	InputHandler::setCallbacks(GLFWwindow* window, Vox& voxInstance)
{
	glfwSetWindowUserPointer(window, (void*)&voxInstance);
	glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		(void)scancode;
		(void)mods;
		Vox* vox = static_cast<Vox*>(glfwGetWindowUserPointer(window));
		InputHandler& handler = vox->getHandler();
		switch(action)
		{
			case GLFW_PRESS:
				handler.keyboard.keysReleased[key] = false;
				handler.keyboard.keysPressed[key] = true;
				break;
			case GLFW_RELEASE:
				handler.keyboard.keysPressed[key] = false;
				handler.keyboard.keysReleased[key] = true;
				break;
			case GLFW_REPEAT:
				handler.keyboard.keysRepeated[key] = true;
				break;
			default:
				break;
		}
	});
	glfwSetMouseButtonCallback(window, [](GLFWwindow* window, int button, int action, int mods)
	{
		(void)mods;
		Vox* vox = static_cast<Vox*>(glfwGetWindowUserPointer(window));
		InputHandler& handler = vox->getHandler();

		switch(action)
		{
			case GLFW_PRESS:
				handler.mouse.buttonsReleased[button] = false;
				handler.mouse.buttonsPressed[button] = true;
				break;
			case GLFW_RELEASE:
				handler.mouse.buttonsPressed[button] = false;
				handler.mouse.buttonsReleased[button] = true;
				break;
			default:
				break;
		}
	});
	glfwSetScrollCallback(window, [](GLFWwindow* window, double xoffset, double yoffset)
	{
		Vox* vox = static_cast<Vox*>(glfwGetWindowUserPointer(window));
		(void)xoffset;
		(void)yoffset;
		(void)vox;
		(void)window;
		// Handle scroll input here if needed
	});
	glfwSetCursorPosCallback(window, [](GLFWwindow* window, double posX, double posY) {
		if (glfwGetWindowAttrib(window, GLFW_FOCUSED) == false)
			return;
		Vox* vox = static_cast<Vox*>(glfwGetWindowUserPointer(window));
		vox->mouseRotation(posX, posY);
	});
	glfwSetWindowFocusCallback(window, [](GLFWwindow* window, int32_t focused) {
		if (focused == GLFW_TRUE)
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		else
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	});
}

void	InputHandler::reset()
{
	keyboard.reset();
	mouse.reset();
}

void	InputHandler::setCursorPos( float posX, float posY ) {
	this->mouse.setCursorPos(posX, posY);
}

void	InputHandler::getCursorPos( float& posX, float& posY ) {
	this->mouse.getCursorPos(posX, posY);
}


struct GlobalUBO
{
	mat4				projectionView{1.0f};
	vec4				ambientLightColor{1.0f, 1.0f, 1.0f, 0.1f};
	vec3				lightPosition{0.0f, -4.0f, -3.0f};
	alignas(16)	vec4	lightColor{1.0f};
};

Vox::Vox( void ) : objModelPath("models/teapot.obj"), camera(vec3{0.0f, 0.0f, Config::cameraDistance}), world(generatorVoxTest3)
{
	globalDescriptorPool = ve::VulkanDescriptorPool::Builder(vulkanDevice)
		.setMaxSets(ve::VulkanSwapChain::MAX_FRAMES_IN_FLIGHT)
		.addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, ve::VulkanSwapChain::MAX_FRAMES_IN_FLIGHT)
		.addPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, ve::VulkanSwapChain::MAX_FRAMES_IN_FLIGHT)
		.build();
	world.generateBufferData();
	inputHandler.setCallbacks(vulkanWindow.getGLFWwindow(), *this);
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
		radians(Config::projectionFov),
		aspectRatio,
		Config::projectionNear,
		Config::projectionFar
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

	while (vulkanWindow.shouldClose() == false)
	{
		glfwPollEvents();
		newTime = std::chrono::high_resolution_clock::now();
		elapsedTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
		currentTime = newTime;
		this->moveCamera(info.camera, elapsedTime);

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
			std::cout << "\rFrames per second: " << static_cast<int> (1.0f / elapsedTime) << ", Frame time: ";
			std::cout << std::chrono::duration<float, std::chrono::milliseconds::period>(newTime - currentTime).count() << "ms " << std::flush;		

			vulkanRenderer.endSwapChainRenderPass(commandBuffer);
			vulkanRenderer.endFrame();
		}
		frameCount++;
	}
	vkDeviceWaitIdle(vulkanDevice.device());
}

void Vox::createObjects( void )
{
	ve::VulkanModel::Builder builder;
	builder.loadModel(objModelPath);
	std::shared_ptr<ve::VulkanModel>	model = ve::VulkanModel::createModel(vulkanDevice, builder); // world.getBuilder()
	ve::VulkanObject 					object = ve::VulkanObject::createVulkanObject();

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

InputHandler const& Vox::getHandler( void ) const noexcept {
	return this->inputHandler;
}

InputHandler& Vox::getHandler( void ) noexcept {
	return this->inputHandler;
}

void Vox::moveCamera( ve::Camera& cameraObj, float deltaTime ) {
	if (this->inputHandler.isKeyPressed(GLFW_KEY_W))
		cameraObj.moveForward(deltaTime * Config::movementSpeed);

	if (this->inputHandler.isKeyPressed(GLFW_KEY_A))
		cameraObj.moveLeft(deltaTime * Config::movementSpeed);

	if (this->inputHandler.isKeyPressed(GLFW_KEY_S))
		cameraObj.moveBackward(deltaTime * Config::movementSpeed);

	if (this->inputHandler.isKeyPressed(GLFW_KEY_D))
		cameraObj.moveRight(deltaTime * Config::movementSpeed);

	if (this->inputHandler.isKeyPressed(GLFW_KEY_E))
		cameraObj.moveUp(deltaTime * Config::movementSpeed);

	if (this->inputHandler.isKeyPressed(GLFW_KEY_Q))
		cameraObj.moveDown(deltaTime * Config::movementSpeed);

	if (this->inputHandler.isKeyPressed(GLFW_KEY_UP))
		cameraObj.rotate(deltaTime * Config::lookSpeed, 0.0f, 0.0f);

	if (this->inputHandler.isKeyPressed(GLFW_KEY_DOWN))
		cameraObj.rotate(-deltaTime * Config::lookSpeed, 0.0f, 0.0f);

	if (this->inputHandler.isKeyPressed(GLFW_KEY_LEFT))
		cameraObj.rotate(0.0f, -deltaTime * Config::lookSpeed, 0.0f);

	if (this->inputHandler.isKeyPressed(GLFW_KEY_RIGHT))
		cameraObj.rotate(0.0f, deltaTime * Config::lookSpeed, 0.0f);
}

void Vox::mouseRotation( float newX, float newY ) noexcept {
	static bool firstRun = true;
	if (firstRun) {
		this->inputHandler.setCursorPos(newX, newY);
		firstRun = false;
	}
	float currX, currY;
	this->inputHandler.getCursorPos(currX, currY);
	float yaw = (newX - currX) * Config::cameraSensitivity;
	float pitch = (currY - newY) * Config::cameraSensitivity;  // reversed since y-coordinates range from bottom to top

	if (pitch > 89.0f)
		pitch =  89.0f;
	else if(pitch < -89.0f)
		pitch = -89.0f;

	this->camera.rotate(pitch, yaw, 0.0f);
	this->inputHandler.setCursorPos(newX, newY);
}

}	// namespace vox
