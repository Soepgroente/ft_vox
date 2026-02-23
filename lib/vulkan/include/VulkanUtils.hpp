#pragma once

#include <functional>
#include <random>
#include <chrono>

#include "Vectors.hpp"

namespace ve {

template <typename T, typename... Rest>
void	hashCombine(std::size_t& seed, const T& v, const Rest&... rest)
{
	seed ^= std::hash<T>{}(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
	(hashCombine(seed, rest), ...);
};

float	randomFloat();
vec3	generateRandomColor();
vec3	generateRandomGreyscale();
vec3	generateSoftGreyscale();

} // namespace ve