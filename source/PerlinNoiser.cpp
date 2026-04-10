#include "PerlinNoiser.hpp"
#include "Vulkan.hpp"		// random number generators


namespace vox {

PerlinNoiser::PerlinNoiser( ui32 seed, float noiseScalar ) : seed{seed}, noiseScalar{noiseScalar}
{
	this->setupPermutations( this->seed );
}

float PerlinNoiser::getPerlinValue( float x, float y, bool skipRange ) const noexcept
{
	x *= this->noiseScalar;
	y *= this->noiseScalar;

	i32 Xi = ((static_cast<i32>(std::floor(x)) & 255) + 256) & 255;
	i32 Yi = ((static_cast<i32>(std::floor(y)) & 255) + 256) & 255;

	float xf = x - std::floor(x);
	float yf = y - std::floor(y);

	vec2 topRight{xf - 1.0f, yf - 1.0f};
	vec2 topLeft{xf, yf - 1.0f};
	vec2 bottomLeft{xf, yf};
	vec2 bottomRight{xf - 1.0f, yf};

	i32 permTopRight = this->permutations[this->permutations[Xi + 1] + Yi + 1];
	i32 permTopLeft = this->permutations[this->permutations[Xi] + Yi + 1];
	i32 permBottomLeft = this->permutations[this->permutations[Xi] + Yi];
	i32 permBottomRight = this->permutations[this->permutations[Xi + 1] + Yi];

	float dotTopRight = vec2::dot(topRight, this->getGradient2D(permTopRight));
	float dotTopLeft = vec2::dot(topLeft, this->getGradient2D(permTopLeft));
	float dotBottomLeft = vec2::dot(bottomLeft, this->getGradient2D(permBottomLeft));
	float dotBottomRight = vec2::dot(bottomRight, this->getGradient2D(permBottomRight));

	float u = this->smooth(xf);
	float v = this->smooth(yf);

	float perlinValue = this->lerp(
		u,
		this->lerp(v, dotBottomLeft, dotTopLeft),
		this->lerp(v, dotBottomRight, dotTopRight)
	);

	if (skipRange == false)
	{
		return (perlinValue + 1.0f) * 0.5f * this->range;
	}
	else
	{
		return (perlinValue + 1.0f) * 0.5f;
	}
}

float PerlinNoiser::getPerlinValue( float x, float y, float z, bool skipRange ) const noexcept
{
	x *= this->noiseScalar;
	y *= this->noiseScalar;
	z *= this->noiseScalar;

	i32 Xi = ((static_cast<i32>(std::floor(x)) & 255) + 256) & 255;
	i32 Yi = ((static_cast<i32>(std::floor(y)) & 255) + 256) & 255;
	i32 Zi = ((static_cast<i32>(std::floor(z)) & 255) + 256) & 255;

	float xf = x - std::floor(x);
	float yf = y - std::floor(y);
	float zf = z - std::floor(z);

	vec3 c000{xf,        yf,        zf};
	vec3 c001{xf,        yf,        zf - 1.0f};
	vec3 c010{xf,        yf - 1.0f, zf};
	vec3 c011{xf,        yf - 1.0f, zf - 1.0f};
	vec3 c100{xf - 1.0f, yf,        zf};
	vec3 c101{xf - 1.0f, yf,        zf - 1.0f};
	vec3 c110{xf - 1.0f, yf - 1.0f, zf};
	vec3 c111{xf - 1.0f, yf - 1.0f, zf - 1.0f};

	i32 p000 = permutations[permutations[permutations[Xi]     + Yi]     + Zi];
	i32 p001 = permutations[permutations[permutations[Xi]     + Yi]     + Zi + 1];
	i32 p010 = permutations[permutations[permutations[Xi]     + Yi + 1] + Zi];
	i32 p011 = permutations[permutations[permutations[Xi]     + Yi + 1] + Zi + 1];
	i32 p100 = permutations[permutations[permutations[Xi + 1] + Yi]     + Zi];
	i32 p101 = permutations[permutations[permutations[Xi + 1] + Yi]     + Zi + 1];
	i32 p110 = permutations[permutations[permutations[Xi + 1] + Yi + 1] + Zi];
	i32 p111 = permutations[permutations[permutations[Xi + 1] + Yi + 1] + Zi + 1];

	float d000 = vec3::dot(c000, this->getGradient3D(p000));
	float d001 = vec3::dot(c001, this->getGradient3D(p001));
	float d010 = vec3::dot(c010, this->getGradient3D(p010));
	float d011 = vec3::dot(c011, this->getGradient3D(p011));
	float d100 = vec3::dot(c100, this->getGradient3D(p100));
	float d101 = vec3::dot(c101, this->getGradient3D(p101));
	float d110 = vec3::dot(c110, this->getGradient3D(p110));
	float d111 = vec3::dot(c111, this->getGradient3D(p111));

	float u = this->smooth(xf);
	float v = this->smooth(yf);
	float w = this->smooth(zf);

	float perlinValue = this->lerp(u,
		this->lerp(v,
			this->lerp(w, d000, d001),
			this->lerp(w, d010, d011)
		),
		this->lerp(v,
			this->lerp(w, d100, d101),
			this->lerp(w, d110, d111)
		)
	);

	if (skipRange == false)
	{
		return (perlinValue + 1.0f) * 0.5f * this->range;
	}
	else
	{
		return (perlinValue + 1.0f) * 0.5f;
	}
}

float PerlinNoiser::getPerlinValueAlt( float x, float y, float z, bool skipRange ) const noexcept
{
	i32 Xi = ((static_cast<i32>(std::floor(x)) & 255) + 256) & 255;
	i32 Yi = ((static_cast<i32>(std::floor(y)) & 255) + 256) & 255;
	i32 Zi = ((static_cast<i32>(std::floor(z)) & 255) + 256) & 255;

	float xf = x - std::floor(x);
	float yf = y - std::floor(y);
	float zf = z - std::floor(z);

	i32 p000 = permutations[permutations[permutations[Xi]     + Yi]     + Zi];
	i32 p001 = permutations[permutations[permutations[Xi]     + Yi]     + Zi + 1];
	i32 p010 = permutations[permutations[permutations[Xi]     + Yi + 1] + Zi];
	i32 p011 = permutations[permutations[permutations[Xi]     + Yi + 1] + Zi + 1];
	i32 p100 = permutations[permutations[permutations[Xi + 1] + Yi]     + Zi];
	i32 p101 = permutations[permutations[permutations[Xi + 1] + Yi]     + Zi + 1];
	i32 p110 = permutations[permutations[permutations[Xi + 1] + Yi + 1] + Zi];
	i32 p111 = permutations[permutations[permutations[Xi + 1] + Yi + 1] + Zi + 1];

	float d000 = this->grad3Dalt(p000, xf,        yf,        zf);
	float d001 = this->grad3Dalt(p001, xf,        yf,        zf - 1.0f);
	float d010 = this->grad3Dalt(p010, xf,        yf - 1.0f, zf);
	float d011 = this->grad3Dalt(p011, xf,        yf - 1.0f, zf - 1.0f);
	float d100 = this->grad3Dalt(p100, xf - 1.0f, yf,        zf);
	float d101 = this->grad3Dalt(p101, xf - 1.0f, yf,        zf - 1.0f);
	float d110 = this->grad3Dalt(p110, xf - 1.0f, yf - 1.0f, zf);
	float d111 = this->grad3Dalt(p111, xf - 1.0f, yf - 1.0f, zf - 1.0f);

	float u = this->smooth(xf);
	float v = this->smooth(yf);
	float w = this->smooth(zf);

	float perlinValue = this->lerp(w,
		this->lerp(v,
			this->lerp(u, d000, d100),
			this->lerp(u, d010, d110)
		),
		this->lerp(v,
			this->lerp(u, d001, d101),
			this->lerp(u, d011, d111)
		)
	);

	if (skipRange == false)
	{
		return (perlinValue + 1.0f) * 0.5f * this->range;
	}
	else
	{
		return (perlinValue + 1.0f) * 0.5f;
	}
}

void PerlinNoiser::setupPermutations( ui32 seed ) noexcept
{
	auto endSequence = this->permutations.begin() + 256;

	std::mt19937 rng(seed);
	std::iota(this->permutations.begin(), endSequence, 0);
	std::shuffle(this->permutations.begin(), endSequence, rng);
	std::copy(this->permutations.begin(), endSequence, endSequence);
}

float PerlinNoiser::lerp(float t, float m1, float m2) const noexcept
{
	return m1 + t * (m2 - m1);
}

float PerlinNoiser::smooth(float t) const noexcept
{
	return ((6 * t - 15) * t + 10) * t * t * t;
}

vec2 const&	PerlinNoiser::getGradient2D( i32 input ) const noexcept
{
	return this->gradients2D[input % this->gradients2D.size()];
}

vec3 const&	PerlinNoiser::getGradient3D( i32 input ) const noexcept
{
	return this->gradients3D[input % this->gradients3D.size()];
}

float PerlinNoiser::grad3Dalt( ui32 hash, float x, float y, float z ) const noexcept
{
	switch (hash & 0xF)
	{
		case 0x0: return  x + y;
		case 0x1: return -x + y;
		case 0x2: return  x - y;
		case 0x3: return -x - y;
		case 0x4: return  x + z;
		case 0x5: return -x + z;
		case 0x6: return  x - z;
		case 0x7: return -x - z;
		case 0x8: return  y + z;
		case 0x9: return -y + z;
		case 0xA: return  y - z;
		case 0xB: return -y - z;
		case 0xC: return  y + x;
		case 0xD: return -y + z;
		case 0xE: return  y - x;
		case 0xF: return -y - z;
		default: return 0;
	}
}

}	// namespace vox