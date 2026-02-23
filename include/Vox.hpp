#pragma once

#include "Vectors.hpp"
#include "Vulkan.hpp"
#include "VoxelWorld.hpp"
#include "Config.hpp"

#include <array>
#include <memory>
#include <vector>

namespace vox {

class Vox;

class InputHandler
{
	public:

	InputHandler() = default;
	~InputHandler() = default;
	InputHandler(const InputHandler& other) = delete;
	InputHandler& operator=(const InputHandler& other) = delete;

	void	setCallbacks(GLFWwindow* window, Vox&);
	void	reset();

	bool	isKeyPressed(int key) const { return keyboard.keysPressed[key]; }
	bool	isKeyReleased(int key) const { return keyboard.keysReleased[key]; }
	bool	isKeyRepeated(int key) const { return keyboard.keysRepeated[key]; }
	bool	isMouseButtonPressed(int button) const { return mouse.buttonsPressed[button]; }
	bool	isMouseButtonReleased(int button) const { return mouse.buttonsReleased[button]; }
	void	setCursorPos( float, float );
	void	getCursorPos( float&, float& );

	private:

	KeyboardInput	keyboard;
	MouseInput		mouse;
};

class Vox
{
	public:
		Vox( void );
		~Vox( void );
		Vox( Vox const& ) = delete;
		Vox( Vox&& ) = delete;
		Vox& operator=( Vox const& ) = delete;
		Vox& operator=( Vox&& ) = delete;

		void run( void );
		void shutdown( void );

		InputHandler const& getHandler( void ) const noexcept;
		InputHandler&		getHandler( void ) noexcept;

		void moveCamera( ve::Camera&, float );
		void mouseRotation( float, float) noexcept;

	private:
		void	createObjects( void );

		ve::VulkanWindow	vulkanWindow{Config::defaultWindowHeight, Config::defaultWindowWidth, "Vox"};
		ve::VulkanDevice	vulkanDevice{vulkanWindow};
		ve::VulkanRenderer	vulkanRenderer{vulkanWindow, vulkanDevice};
		std::unique_ptr<ve::VulkanDescriptorPool>	globalDescriptorPool{};

		std::string						objModelPath;
		ve::Camera						camera;
		ve::VulkanObject::Map			objects;
		std::vector<ve::VulkanTexture>	textures;
		VoxelWorld						world;
		InputHandler					inputHandler;
};

}
