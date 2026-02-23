#pragma once

#include <ostream>

class vec3i
{
	public:

	union
	{
		int	data[3];
		struct
		{
			int	x;
			int	y;
			int	z;
		};
	};

	vec3i() : x(0), y(0), z(0) {}
	vec3i(int val) : x(val), y(val), z(val) {}
	vec3i(int x, int y, int z) : x(x), y(y), z(z) {}
	vec3i(const vec3i& other) : x(other.x), y(other.y), z(other.z) {}
	vec3i&	operator=(const vec3i& other);
	~vec3i() = default;

	vec3i	operator+(const vec3i& other) const noexcept { return vec3i(x + other.x, y + other.y, z + other.z); }
	vec3i	operator-(const vec3i& other) const noexcept { return vec3i(x - other.x, y - other.y, z - other.z); }
	vec3i&	operator+=(const vec3i& other) noexcept { x += other.x; y += other.y, z += other.z; return *this; }
	vec3i&	operator-=(const vec3i& other) noexcept { x -= other.x; y -= other.y; z -= other.z; return *this; }

	bool	operator==(const vec3i& other) const noexcept { return x == other.x && y == other.y && z == other.z; }
	bool	operator!=(const vec3i& other) const noexcept { return !(*this == other); }
	bool	operator<(const vec3i& other) const noexcept;
	bool	operator<=(const vec3i& other) const noexcept { return *this < other || *this == other; }
	bool	operator>(const vec3i& other) const noexcept { return !(*this <= other); }
	bool	operator>=(const vec3i& other) const noexcept { return !(*this < other); }

	int&		operator[](int index) noexcept { return data[index]; }
	const int&	operator[](int index) const noexcept { return data[index]; }
	
	vec3i	clone() const noexcept { return vec3i(x, y, z); }
	
	static vec3i	zero() noexcept { return vec3i(0.0f, 0.0f, 0.0f); }

	friend vec3i	operator-(const vec3i& v) noexcept { return vec3i(-v.x, -v.y, -v.z); }

	private:
};

std::ostream&	operator<<(std::ostream& os, const vec3i& v);
