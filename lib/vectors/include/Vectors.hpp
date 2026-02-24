#pragma once

#include "Vec2.hpp"
#include "Vec3.hpp"
#include "Vec3i.hpp"
#include "Vec4.hpp"
#include "Quat.hpp"
#include "Mat3.hpp"
#include "Mat4.hpp"

#include <cmath>
#include <limits>

constexpr float	pi() noexcept { return 3.14159265358979323846f; }
constexpr float	two_pi() noexcept { return 2.0f * pi(); }
constexpr float	half_pi() noexcept { return pi() / 2.0f; }
constexpr float	epsilon() noexcept { return std::numeric_limits<float>::epsilon(); }

union FloatIntUnion
{
	float	f;
	int32_t	i;
};

float	fastInverseSqrt(float number) noexcept;
float	radians(float degrees) noexcept;
float	radiansToDegrees(float radians) noexcept;