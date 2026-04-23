#pragma once

#include "Vectors.hpp"


namespace vox {

struct Config
{
	static constexpr ui32	defaultWindowWidth = 1300;
	static constexpr ui32	defaultWindowHeight = 1300;
	static constexpr ui32	minimumViewingDistance = 160;

	static constexpr ui32	worldSeed = 0U;

	static constexpr vec3 cameraStartPos{25.0f, 7.0f, 25.0f};
	static constexpr vec3 cameraForward{20.0f, 5.0f, 20.0f};

	static constexpr vec3 sunPos{-15.0f, 10.0f, -15.0f};

	static constexpr vec3 lightAmbientColor{0.2f, 0.2f, 0.2f};
	static constexpr vec3 lightColor{0.5f, 0.5f, 0.5f};
	static constexpr vec3 lightSpecularColor{0.1f, 0.1f, 0.1f};

	static constexpr i32	chunkLength = 16U;
	static constexpr i32	chunkHeight = 256U;
	static constexpr i32	seaLevel = 64U;

	static constexpr float	movementSpeed = 20.0f;
	static constexpr float	lookSpeed = 25.0f;

	// static constexpr char terrainVertShaderPath[] = "build/basic.vert.spv";
	// static constexpr char terrainFragShaderPath[] = "build/basic.frag.spv";
	// static constexpr char skyboxVertShaderPath[] = "build/skybox.vert.spv";
	// static constexpr char skyboxFragShaderPath[] = "build/skybox.frag.spv";
	// static constexpr char lightVertShaderPath[] = "build/light_base.vert.spv";
	// static constexpr char lightFragShaderPath[] = "build/light_base.frag.spv";

	static constexpr char simpleVertShaderPath[] = "build/light_test.vert.spv";
	static constexpr char simpleFragShaderPath[] = "build/light_test.frag.spv";
	static constexpr char sunVertShaderPath[] = "build/sun.vert.spv";
	static constexpr char sunFragShaderPath[] = "build/sun.frag.spv";

	static constexpr char textureStonePath[] = "textures/texture_stone_mono.jpeg";
	static constexpr char textureWaterPath[] = "textures/texture_water_mono.jpeg";
	static constexpr char textureDirtPath[] = "textures/texture_dirt_atlas.jpeg";
	static constexpr char textureSkyboxPath[] = "textures/skybox1.png";
};

} // namespace vox
