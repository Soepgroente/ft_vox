#pragma once

#include <array>
#include <cstdint>

#include "Vectors.hpp"


namespace vox {

using ui8 = uint8_t;
using i32 = int32_t;
using ui32 = uint32_t;

class PerlinNoiser
{
	public:
		PerlinNoiser( void ) = delete;
		PerlinNoiser( ui32 seed, float noiseScalar );

		float	getPerlinValue( float x, float y ) const noexcept;
		float	getPerlinValue( float x, float y, float z ) const noexcept;
		float	getPerlinValueAlt( float x, float y, float z ) const noexcept;

		ui32	getSeed( void ) const noexcept { return this->seed; };
		void	setSeed( uint32_t seed ) noexcept { this->seed = seed; };

	private:
		void		setupPermutations( ui32 seed ) noexcept;
		float		lerp(float t, float m1, float m2) const noexcept;
		float		smooth(float t) const noexcept;
		vec2 const&	getGradient2D( i32 input ) const noexcept;
		vec3 const&	getGradient3D( i32 input ) const noexcept;
		float		grad3Dalt( ui32 hash, float x, float y, float z ) const noexcept;
		
		ui32						seed;
		float						noiseScalar;
		std::array<ui32,512>		permutations;
		std::array<vec2,4> const	gradients2D{
			vec2{ 1.0f,  1.0f},
			vec2{-1.0f,  1.0f},
			vec2{-1.0f, -1.0f},
			vec2{ 1.0f, -1.0f}
		};
		std::array<vec3,12> const	gradients3D{
			vec3{1.0f,  1.0f,  0.0f}, vec3{-1.0f,  1.0f,  0.0f},
			vec3{1.0f, -1.0f,  0.0f}, vec3{-1.0f, -1.0f,  0.0f},
			vec3{1.0f,  0.0f,  1.0f}, vec3{-1.0f,  0.0f,  1.0f},
			vec3{1.0f,  0.0f, -1.0f}, vec3{-1.0f,  0.0f, -1.0f},
			vec3{0.0f,  1.0f,  1.0f}, vec3{ 0.0f, -1.0f,  1.0f},
			vec3{0.0f,  1.0f, -1.0f}, vec3{ 0.0f, -1.0f, -1.0f}
		};
};

}	// namespace vox