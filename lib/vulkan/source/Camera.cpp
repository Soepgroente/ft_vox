#include "Camera.hpp"
#include "Vectors.hpp"

#include <cassert>
#include <cmath>
#include <iostream>

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

void	Camera::setViewMatrix( void ) noexcept
{
	vec3 cameraTarget = this->_position + this->_forward;		// position that the camera is watching
	this->_cameraForward = (cameraTarget - this->_position).normalize();
	this->_cameraLeft = vec3::cross(this->_cameraForward, this->__up).normalize();
	this->_cameraUp = vec3::cross(this->_cameraLeft, this->_cameraForward);

	this->viewMatrix = mat4{
		{this->_cameraLeft.x,  this->_cameraUp.x,  this->_cameraForward.x,  0.0f},
		{this->_cameraLeft.y,  this->_cameraUp.y,  this->_cameraForward.y,  0.0f},
		{this->_cameraLeft.z,  this->_cameraUp.z,  this->_cameraForward.z,  0.0f},
		{-vec3::dot(this->_cameraLeft, this->_position), -vec3::dot(this->_cameraUp, this->_position), -vec3::dot(this->_cameraForward, this->_position), 1.0f}
	};
}

const mat4&	Camera::getProjectionMatrix( bool recalculate ) noexcept {
	if (recalculate)
		this->setViewMatrix();
	return this->projectionMatrix;
}

const mat4&	Camera::getViewMatrix( bool recalculate ) noexcept {
	if (recalculate)
		this->setViewMatrix();
	return this->viewMatrix;
}

vec3 const& Camera::getCameraPos( void ) noexcept {
	return this->_position;
}

void Camera::moveForward( float delta ) noexcept {
	vec3 progression = this->_position + this->_cameraForward * delta;
	if (progression.x < this->_limits.x)
		progression.x = this->_limits.x;
	if (progression.y < this->_limits.y)
		progression.y = this->_limits.y;
	if (progression.z < this->_limits.z)
		progression.z = this->_limits.z;
	this->_position = progression;
}

void Camera::moveBackward( float delta ) noexcept {
	vec3 progression = this->_position - this->_cameraForward * delta;
	if (progression.x < this->_limits.x)
		progression.x = this->_limits.x;
	if (progression.y < this->_limits.y)
		progression.y = this->_limits.y;
	if (progression.z < this->_limits.z)
		progression.z = this->_limits.z;
	this->_position = progression;
}

void Camera::moveRight( float delta ) noexcept {
	vec3 progression = this->_position + this->_cameraLeft * delta;
	if (progression.x < this->_limits.x)
		progression.x = this->_limits.x;
	if (progression.y < this->_limits.y)
		progression.y = this->_limits.y;
	if (progression.z < this->_limits.z)
		progression.z = this->_limits.z;
	this->_position = progression;
}

void Camera::moveLeft( float delta ) noexcept {
	vec3 progression = this->_position - this->_cameraLeft * delta;
	if (progression.x < this->_limits.x)
		progression.x = this->_limits.x;
	if (progression.y < this->_limits.y)
		progression.y = this->_limits.y;
	if (progression.z < this->_limits.z)
		progression.z = this->_limits.z;
	this->_position = progression;
}

void Camera::moveUp( float delta ) noexcept {
	vec3 progression = this->_position + this->_cameraUp * delta;
	if (progression.x < this->_limits.x)
		progression.x = this->_limits.x;
	if (progression.y < this->_limits.y)
		progression.y = this->_limits.y;
	if (progression.z < this->_limits.z)
		progression.z = this->_limits.z;
	this->_position = progression;
}

void Camera::moveDown( float delta ) noexcept {
	vec3 progression = this->_position - this->_cameraUp * delta;
	if (progression.x < this->_limits.x)
		progression.x = this->_limits.x;
	if (progression.y < this->_limits.y)
		progression.y = this->_limits.y;
	if (progression.z < this->_limits.z)
		progression.z = this->_limits.z;
	this->_position = progression;
}

void Camera::rotate( float pitch, float yaw, float roll ) noexcept {
	pitch = radians(pitch / 2.0f);		// vertical rotation: cameraLeft is the axis
	yaw = radians(yaw / 2.0f);			// horizontal rotation: up is the axis
	roll = radians(roll / 2.0f);		// frontal rotation: z is the axis

	quat qPitch(pitch, this->_cameraLeft);
	quat qYaw(-yaw, this->__up);
	quat qRoll(roll, this->_forward);

	quat q = quat::product(qPitch, quat::product(qYaw, qRoll)).normalize();		// rotation order: roll -> yaw -> pitch
	quat qForward{0.0f, this->_forward.x, this->_forward.y, this->_forward.z};
	quat qForwardRotated = quat::product(quat::product(q, qForward), q.conjugated());
	this->_forward = qForwardRotated.normalized().vector();

	if (roll != 0.0f) {		// with a roll rotation the world up has to be changed
		quat qUp{0.0f, this->__up.x, this->__up.y, this->__up.z};
		quat qUpRotated = quat::product(quat::product(q, qUp), q.conjugated());
		this->__up = qUpRotated.normalized().vector();
	}
}


}	// namespace ve