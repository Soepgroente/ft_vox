#pragma once
#include "Vulkan.hpp"

#include <limits>


namespace vox {

struct Config
{
	static constexpr uint32_t	defaultWindowWidth = 1300;
	static constexpr uint32_t	defaultWindowHeight = 1300;

	static constexpr vec3ui	worldLimits{
		16384U,
		16384U,
		256U
	};
	static constexpr vec3	gridLimits{
		-100000.0f,
		-100000.0f,
		1.5f
	};

	static constexpr uint32_t	gridSize = 32U;
	static constexpr vec3		centerGridPos{		// NB this is ugly, z should depend on ground lvl
		gridSize / 2.0f,
		gridSize / 2.0f,
		2.0f
	};

	static constexpr float	movementSpeed = 10.0f;
	static constexpr float	lookSpeed = 75.0f;

	static constexpr char const	vertShaderPath[] = "build/basic.vert.spv";
	static constexpr char const	fragShaderPath[] = "build/basic.frag.spv";
};

} // namespace vox
