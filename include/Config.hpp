#pragma once

#include "Vectors.hpp"


namespace vox {

struct Config
{
	static constexpr ui32	defaultWindowWidth = 1300;
	static constexpr ui32	defaultWindowHeight = 1300;
	static constexpr ui32	minimumViewingDistance = 160;

	static constexpr ui32	worldSeed = 0U;

	static constexpr vec3 cameraStartPos{10.0f, 5.0f, 10.0f};
	static constexpr vec3 cameraForward{20.0f, 5.0f, 20.0f};

	static constexpr vec3 sunPos{15.0f, 10.0f, 15.0f};
	static constexpr vec4 lightColor{1.0f, 1.0f, 1.0f, 1.0f};

	// Phong lighting model, a combination of the following:
	// Ambient lighting: even when it is dark there is usually still some light somewhere in the world
	// 		(the moon, a distant light) so objects are almost never completely dark. To simulate this
	// 		we use an ambient lighting constant that always gives the object some color.
	// Diffuse lighting: simulates the directional impact a light object has on an object. This is the
	// 		most visually significant component of the lighting model. The more a part of an object
	// 		faces the light source, the brighter it becomes.
	// Specular lighting: simulates the bright spot of a light that appears on shiny objects. Specular
	// 		highlights are more inclined to the color of the light than the color of the object.
	static constexpr ve::MeshMaterial dirtMaterial{
		vec4(0.1f),						// ambientClr
		vec4(0.4f, 0.8f, 0.3f, 1.0f),	// diffuseClr
		vec4(0.4f),						// specularClr
		8,								// shininess
		1.0f,							// opacity
		1,								// refractionIndex
		2								// illuminationModel
	};

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
