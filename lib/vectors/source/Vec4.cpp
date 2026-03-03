#include "Vec4.hpp"

#include <cmath>

vec4&	vec4::operator=(const vec4& other)
{
	if (this != &other)
	{
		x = other.x;
		y = other.y;
		z = other.z;
		w = other.w;
	}
	return *this;
}