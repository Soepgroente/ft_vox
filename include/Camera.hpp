#pragma once

#include "Vectors.hpp"

namespace vox {

struct CameraSettings {
	static constexpr float	projectionFov = 50.0f;
	static constexpr float	projectionNear = .1f;
	static constexpr float	projectionFar = 1000.0f;
	static constexpr float	cameraDistance = 50.0f;
	static constexpr float	cameraSensitivity = 0.1f;
	static constexpr vec3	cameraUpDefault{0.0f, 1.0f, 0.0f};
};


class Camera
{
	public:
		Camera( vec3 const& pos, vec3 const& forward, vec3 const& limits ) : 
			_position(pos),
			_forward(forward),
			_limits(limits),
			// the up depends on what axis the camera is watching (so it depends on forward): 
			// camera looks (forward) along x or z -> __up is non-null in y, 
			// camera looks (forward) along y -> __up is non-null in z
			__up(CameraSettings::cameraUpDefault) {};

		void	setOrthographicProjection(float left, float right, float top, float bottom, float near, float far);
		void	setPerspectiveProjection(float fovy, float aspect, float near, float far);
		void	setViewMatrix( void ) noexcept;

		const mat4&	getProjectionMatrix( void ) const noexcept;
		const mat4&	getViewMatrix( void ) noexcept;

		vec3 const&	getCameraPos( void ) noexcept;

		void	moveForward( float ) noexcept;
		void	moveBackward( float ) noexcept;
		void	moveRight( float ) noexcept;
		void	moveLeft( float ) noexcept;
		void	moveUp( float ) noexcept;
		void	moveDown( float ) noexcept;
		void	rotate( float, float, float ) noexcept;

	private:
		mat4	projectionMatrix{1.0f};
		mat4	viewMatrix{1.0f};

		vec3	_position;		// position of the camera
		vec3	_forward;		// where the camera is pointing
		vec3	_limits;		// to avoid the camera to move below this coordinate
		vec3	__up;			// general up
		vec3	_cameraForward;	// z axis of the camera
		vec3	_cameraLeft;	// x axis of the camera
		vec3	_cameraUp;		// y axis of the camera

		float	_currentPitch = 0.0f;	// to avoid vertical rotations > 90° or < -90°
};

}	// namespace vox