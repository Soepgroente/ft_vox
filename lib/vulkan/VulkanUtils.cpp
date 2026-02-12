#include "VulkanUtils.hpp"

static float	randomFloat()
{
	static std::default_random_engine	engine(std::chrono::system_clock::now().time_since_epoch().count());
	static std::uniform_real_distribution<float>	distribution(0.0f, 1.0f);

	return distribution(engine);
}

namespace ve {

vec3	generateRandomColor()
{
	return vec3(randomFloat(), randomFloat(), randomFloat());
}

vec3	generateRandomGreyscale()
{
	float	grey = randomFloat();

	return vec3(grey, grey, grey);
}

vec3	generateSoftGreyscale()
{
	static std::default_random_engine	engine(std::chrono::system_clock::now().time_since_epoch().count());
	static std::uniform_real_distribution<float>	distribution(0.25f, 0.4f);
	float	grey = distribution(engine);

	return vec3(grey, grey, grey);
}

}