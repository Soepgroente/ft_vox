#pragma once

namespace vox {

struct Config
{
	static constexpr uint32_t	defaultWindowWidth = 1300;
	static constexpr uint32_t	defaultWindowHeight = 1300;
	static constexpr uint32_t	worldSize = 64;

	static constexpr float	movementSpeed = 10.0f;
	static constexpr float	lookSpeed = 75.0f;

	static constexpr char const	vertShaderPath[] = "build/basic.vert.spv";
	static constexpr char const	fragShaderPath[] = "build/basic.frag.spv";
};

} // namespace vox
