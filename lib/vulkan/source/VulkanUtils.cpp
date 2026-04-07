#include "VulkanUtils.hpp"

#include <fstream>
#include <string>


namespace ve {

float randomFloat()
{
	static std::default_random_engine	engine(std::chrono::system_clock::now().time_since_epoch().count());
	static std::uniform_real_distribution<float>	distribution(0.0f, 1.0f);

	return distribution(engine);
}

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


std::vector<char> readFile(std::string const& filePath)
{
	std::ifstream file(filePath, std::ios::ate | std::ios::binary);

	if (file.is_open() == false)
	{
		throw std::runtime_error("failed to open file: " + filePath);
	}
	size_t fileSize = static_cast<size_t>(file.tellg());
	std::vector<char>	buffer(fileSize);

	file.seekg(0);
	file.read(buffer.data(), fileSize);
	file.close();

	return buffer;
}
}