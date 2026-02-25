#include "Vec3ui.hpp"

#include <algorithm>

vec3ui&	vec3ui::operator=(const vec3ui& other)
{
	if (this != &other)
	{
		x = other.x;
		y = other.y;
		z = other.z;
	}
	return *this;
}

bool	vec3ui::operator<(const vec3ui& other) const noexcept
{
	if (x < other.x)
	{
		return true;
	}
	if (x > other.x)
	{
		return false;	
	}
	if (y < other.y)
	{
		return true;
	}
	if (y > other.y)
	{
		return false;
	}
	return z < other.z;
}

std::ostream&	operator<<(std::ostream& os, const vec3ui& v)
{
	os << "vec3ui(" << v.x << ", " << v.y << ", " << v.z << ")";
	return os;
}