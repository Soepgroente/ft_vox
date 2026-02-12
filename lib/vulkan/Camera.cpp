#include "Camera.hpp"
#include "Vectors.hpp"

#include <cassert>
#include <cmath>

namespace ve {

void	Camera::setOrthographicProjection(float left, float right, float top, float bottom, float near, float far)
{
	projectionMatrix = mat4{
		{2.0f / (right - left), 0.0f, 0.0f, 0.0f},
		{0.0f, 2.0f / (bottom - top), 0.0f, 0.0f},
		{0.0f, 0.0f, 1.0f / (far - near), 0.0f},
		{-(right + left) / (right - left), -(bottom + top) / (bottom - top), -near / (far - near), 1.0f}
	};
}

void	Camera::setPerspectiveProjection(float fovy, float aspect, float near, float far)
{
	assert(std::abs(aspect - epsilon()) > 0.0f);
	const float tanHalfFovy = std::tan(fovy / 2.f);

	projectionMatrix = mat4{
		{1.f / (aspect * tanHalfFovy), 0.0f, 0.0f, 0.0f},
		{0.0f, -1.f / tanHalfFovy, 0.0f, 0.0f},
		{0.0f, 0.0f, far / (far - near), 1.0f},
		{0.0f, 0.0f, -(far * near) / (far - near), 0.0f}
	};
}

void	Camera::setViewDirection(const vec3& position, const vec3& direction, const vec3& up)
{
	const vec3 w = direction.normalized();
	const vec3 u = vec3::cross(w, up).normalized();
	const vec3 v = vec3::cross(u, w);

	viewMatrix = mat4{
		{u.x, v.x, w.x, 0.0f},
		{u.y, v.y, w.y, 0.0f},
		{u.z, v.z, w.z, 0.0f},
		{-vec3::dot(u, position), -vec3::dot(v, position), -vec3::dot(w, position), 1.0f}
	};
}

void	Camera::setViewTarget(const vec3& position, const vec3& target, const vec3& up)
{
	setViewDirection(position, target - position, up);
}

void	Camera::setViewYXZ(const vec3& position, const vec3& rotation)
{
	const float c3 = std::cos(rotation.z);
	const float s3 = std::sin(rotation.z);
	const float c2 = std::cos(rotation.x);
	const float s2 = std::sin(rotation.x);
	const float c1 = std::cos(rotation.y);
	const float s1 = std::sin(rotation.y);

	const vec3 u{(c1 * c3 + s1 * s2 * s3), (c2 * s3), (c1 * s2 * s3 - c3 * s1)};
	const vec3 v{(c3 * s1 * s2 - c1 * s3), (c2 * c3), (c1 * c3 * s2 + s1 * s3)};
	const vec3 w{(c2 * s1), (-s2), (c1 * c2)};

	viewMatrix = mat4{
		{u.x, v.x, w.x, 0.0f},
		{u.y, v.y, w.y, 0.0f},
		{u.z, v.z, w.z, 0.0f},
		{-vec3::dot(u, position), -vec3::dot(v, position), -vec3::dot(w, position), 1.0f}
	};
}

}	// namespace ve