#pragma once

#include "Vectors.hpp"
#include "Vulkan.hpp"
#include "Config.hpp"

#include <array>
#include <memory>
#include <vector>

namespace vox {

class Vox
{
	public:
        bool initialize( void );
        void run( void );
        void shutdown( void );
};

}	// namespace vox
