#pragma once

#include "Vulkan.hpp"


namespace vox {

using ui32 = uint32_t;

struct Config
{
	static constexpr ui32	defaultWindowWidth = 1300;
	static constexpr ui32	defaultWindowHeight = 1300;
	static constexpr ui32	minimumViewingDistance = 160;

	static constexpr uint32_t	worldSeed = 0U;

	static constexpr vec3 cameraStartPos{165.0f, 225.0f, 165.0f};
	static constexpr vec3 cameraForward{0.0f, -5.0f, -10.0f};

	static constexpr vec3 sunPos{10.0f, 20.0f, 10.0f};
	static constexpr vec4 lightColor{1.0f, 1.0f, 1.0f, 1.0f};

	static constexpr ve::MeshMaterial dirtMaterial{
		vec4(0.1f),		// ambientClr
		vec4(0.8f),		// diffuseClr
		vec4(0.4f),		// specularClr
		8,				// shininess
		1.0f,			// opacity
		1,				// refractionIndex
		2				// illuminationModel
	};

	static constexpr i32	chunkLength = 16U;
	static constexpr i32	chunkHeight = 256U;

	static constexpr float	movementSpeed = 100.0f;
	static constexpr float	lookSpeed = 75.0f;

	static constexpr char terrainVertShaderPath[] = "build/basic.vert.spv";
	static constexpr char terrainFragShaderPath[] = "build/basic.frag.spv";
	static constexpr char skyboxVertShaderPath[] = "build/skybox.vert.spv";
	static constexpr char skyboxFragShaderPath[] = "build/skybox.frag.spv";
	static constexpr char lightVertShaderPath[] = "build/light_base.vert.spv";
	static constexpr char lightFragShaderPath[] = "build/light_base.frag.spv";

	static constexpr char textureStonePath[] = "textures/texture_stone_mono.jpeg";
	static constexpr char textureWaterPath[] = "textures/texture_water_mono.jpeg";
	static constexpr char textureDirtPath[] = "textures/texture_dirt_atlas.jpeg";
	static constexpr char textureSkyboxPath[] = "textures/skybox1.png";
};

} // namespace vox
