#pragma once

#include "Vectors.hpp"


namespace vox {

struct Config
{
	static constexpr uint32_t	defaultWindowWidth = 1300;
	static constexpr uint32_t	defaultWindowHeight = 1300;
	static constexpr uint32_t	minimumViewingDistance = 160;

	static constexpr vec3ui	mapLimits{
		16384U,
		16384U,
		256U
	};

	static constexpr vec3	cameraLimitsMov{
		-100000.0f,
		-256.0f,
		-100000.0f
	};

	static constexpr uint32_t	worldSize = 16U;
	static constexpr uint32_t	worldHeight = 256U;
	static constexpr vec3		startingPos{		// NB this is ugly, y should depend on ground lvl
		worldSize / 2.0f + 0.5f,
		128.0f,
		worldSize / 2.0f + 0.5f
	};

	static constexpr float	movementSpeed = 100.0f;
	static constexpr float	lookSpeed = 75.0f;

	static constexpr char const	vertShaderPath[] = "build/basic.vert.spv";
	static constexpr char const	fragShaderPath[] = "build/basic.frag.spv";

	static constexpr char const	texture1VoxelPath[] = "textures/texture_stone_mono.jpeg";
	static constexpr char const	texture2VoxelPath[] = "textures/texture_dirt_atlas.jpeg";
	static constexpr float	noiseScalar = 0.01f;
};

} // namespace vox
