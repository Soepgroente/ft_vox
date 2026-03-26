#pragma once

#include "Vectors.hpp"


namespace vox {

using ui32 = uint32_t;

struct Config
{
	static constexpr ui32	defaultWindowWidth = 1300;
	static constexpr ui32	defaultWindowHeight = 1300;
	static constexpr ui32	minimumViewingDistance = 160;

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

	static constexpr ui32	chunkLength = 16U;
	static constexpr ui32	chunkHeight = 256U;
	static constexpr ui32	seaLevel = 64U;
	static constexpr vec3		startingPos{		// NB this is ugly, y should depend on ground lvl
		chunkLength / 2.0f + 0.5f,
		128.0f,
		chunkLength / 2.0f + 0.5f
	};

	static constexpr float	movementSpeed = 100.0f;
	static constexpr float	lookSpeed = 75.0f;

	static constexpr char const	vertShaderPath[] = "build/basic.vert.spv";
	static constexpr char const	fragShaderPath[] = "build/basic.frag.spv";

	static constexpr char const	texture1VoxelPath[] = "textures/texture_stone_mono.jpeg";
	static constexpr char const	texture2VoxelPath[] = "textures/texture_dirt_atlas.jpeg";
};

} // namespace vox
