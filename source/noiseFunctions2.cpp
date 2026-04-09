#include "noiseFunctions.hpp"


namespace vox {

static i32 getGradientIndex( i32 input)
{
	return input % 3;
}

static float lerp(float t, float m1, float m2)
{
	return m1 + t * (m2 - m1);
}

static float serp(float t)
{
	return ((6 * t - 15) * t + 10) * t * t * t;
}

float perlin2(float x, float y)
{
	i32 Xi = static_cast<i32>(std::floor(x)) % 256;
	i32 Yi = static_cast<i32>(std::floor(y)) % 256;

	float xf = x - std::floor(x);
	float yf = y - std::floor(y);

	vec2 topRight{xf - 1.0f, yf - 1.0f};
	vec2 topLeft{xf, yf - 1.0f};
	vec2 bottomLeft{xf, yf};
	vec2 bottomRight{xf - 1.0f, yf};

	i32 permTopRight = permutations[permutations[Xi + 1] + Yi + 1];
	i32 permTopLeft = permutations[permutations[Xi] + Yi + 1];
	i32 permBottomLeft = permutations[permutations[Xi] + Yi];
	i32 permBottomRight = permutations[permutations[Xi + 1] + Yi];

	float dotTopRight = vec2::dot(topRight, gradients[getGradientIndex(permTopRight)]);
	float dotTopLeft = vec2::dot(topLeft, gradients[getGradientIndex(permTopLeft)]);
	float dotBottomLeft = vec2::dot(bottomLeft, gradients[getGradientIndex(permBottomLeft)]);
	float dotBottomRight = vec2::dot(bottomRight, gradients[getGradientIndex(permBottomRight)]);

	float u = serp(xf);
	float v = serp(yf);

	return lerp(
		u,
		lerp(v, dotBottomLeft, dotTopLeft),
		lerp(v, dotBottomRight, dotTopRight)
	);
}

float octavePerlin2(float x, float y, int octaves, float persistence)
{
	float	total = 0;
	float	frequency = 1;
	float	amplitude = 1;
	float	maxValue = 0;

	for (int i = 0; i < octaves; i++)
	{
		total += perlin2(x * frequency, y * frequency) * amplitude;
		
		maxValue += amplitude;
		
		amplitude *= persistence;
		frequency *= 2;
	}
	
	return total / maxValue;
}

}	// namespace vox