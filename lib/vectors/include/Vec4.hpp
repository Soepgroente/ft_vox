#pragma once

#include <ostream>

class vec4
{
	public:

	union
	{
		struct
		{
			float	x;
			float	y;
			float	z;
			float	w;
		};
		float	data[4];
	};

	vec4() : x(0.0f), y(0.0f), z(0.0f), w(0.0f) {}
	vec4(float val) : x(val), y(val), z(val), w(val) {}
	vec4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
	vec4(const vec4& other) : x(other.x), y(other.y), z(other.z), w(other.w) {}
	vec4&	operator=(const vec4& other);
	~vec4() = default;

	vec4	operator+(const vec4& other) const noexcept { return vec4(x + other.x, y + other.y, z + other.z, w + other.w); }
	vec4	operator-(const vec4& other) const noexcept { return vec4(x - other.x, y - other.y, z - other.z, w - other.w); }
	vec4	operator*(float scalar) const noexcept { return vec4(x * scalar, y * scalar, z * scalar, w * scalar); }
	vec4	operator/(float scalar) const noexcept { return vec4(x / scalar, y / scalar, z / scalar, w / scalar); }
	vec4&	operator+=(const vec4& other) noexcept { x += other.x; y += other.y, z += other.z; w += other.w; return *this; }
	vec4&	operator-=(const vec4& other) noexcept { x -= other.x; y -= other.y; z -= other.z; w -= other.w; return *this; }
	vec4&	operator*=(float scalar) noexcept { x *= scalar; y *= scalar; z *= scalar; w *= scalar; return *this; }
	vec4&	operator/=(float scalar) noexcept { x /= scalar; y /= scalar; z /= scalar; w /= scalar; return *this; }
	bool	operator==(const vec4& other) const noexcept { return x == other.x && y == other.y && z == other.z && w == other.w; }
	bool	operator!=(const vec4& other) const noexcept { return !(*this == other); }

	float&			operator[](int index) noexcept { return data[index]; }
	const float&	operator[](int index) const noexcept { return data[index]; }
};