#pragma once
#include "Vulkan.hpp"

#include <limits>


namespace vox {

struct Config
{
	static constexpr uint32_t	defaultWindowWidth = 1300;
	static constexpr uint32_t	defaultWindowHeight = 1300;

	static constexpr vec3ui	mapLimits{
		16384U,
		16384U,
		256U
	};

	static constexpr vec3	worldLimits{
		-100000.0f,
		1.5f,
		-100000.0f
	};

	static constexpr uint32_t	worldSize = 32U;
	static constexpr vec3		startingPos{		// NB this is ugly, y should depend on ground lvl
		worldSize / 2.0f,
		2.0f,
		worldSize / 2.0f
	};

	static constexpr uint32_t	maxWorldsStored = 20U;

	static constexpr float	movementSpeed = 10.0f;
	static constexpr float	lookSpeed = 75.0f;

	static constexpr char const	vertShaderPath[] = "build/basic.vert.spv";
	static constexpr char const	fragShaderPath[] = "build/basic.frag.spv";
};

} // namespace vox
