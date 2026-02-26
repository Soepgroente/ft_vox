#pragma once

namespace vox {

struct Config
{
	static constexpr uint32_t	defaultWindowWidth = 1300;
	static constexpr uint32_t	defaultWindowHeight = 1300;
	static constexpr uint32_t	worldSize = 64;

	static constexpr float	movementSpeed = 10.0f;
	static constexpr float	lookSpeed = 75.0f;

	static constexpr float	projectionFov = 50.0f;
	static constexpr float	projectionNear = .1f;
	static constexpr float	projectionFar = 1000.0f;
	static constexpr float	cameraDistance = 50.0f;
	static constexpr float	cameraSensitivity = 0.1f;
};

} // namespace vox
