#pragma once

#include "Vectors.hpp"

namespace ve {

class Camera
{
	public:

	void	setOrthographicProjection(float left, float right, float top, float bottom, float near, float far);
	void	setPerspectiveProjection(float fovy, float aspect, float near, float far);

	void	setViewDirection(const vec3& position, const vec3& direction, const vec3& up = vec3{0.0f, -1.0f, 0.0f});
	void	setViewTarget(const vec3& position, const vec3& target, const vec3& up = vec3{0.0f, -1.0f, 0.0f});
	void	setViewYXZ(const vec3& position, const vec3& rotation);

	const mat4&	getProjectionMatrix() const noexcept { return projectionMatrix; }
	const mat4&	getViewMatrix() const noexcept { return viewMatrix; }

	private:

	mat4	projectionMatrix{1.0f};
	mat4	viewMatrix{1.0f};
};

} // namespace ve