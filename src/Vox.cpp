#include "Vox.hpp"

#include <chrono>
#include <random>

namespace vox {

bool	Vox::initialize( void )
{
	inputHandler.setCallbacks(vulkanWindow.getGLFWwindow());
    return true;
}

void Vox::run( void ) {

}

void Vox::shutdown( void ) {

}

}	// namespace vox
