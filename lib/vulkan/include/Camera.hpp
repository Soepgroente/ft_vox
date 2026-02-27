#pragma once

#include "Vectors.hpp"

namespace ve {

struct CameraSettings {
	static constexpr float	projectionFov = 50.0f;
	static constexpr float	projectionNear = .1f;
	static constexpr float	projectionFar = 1000.0f;
	static constexpr float	cameraDistance = 50.0f;
	static constexpr float	cameraSensitivity = 0.1f;
};

class Camera
{
	public:
		Camera( vec3 const& pos ) : 
			_position(pos),
			_forward(pos * -1),
			__up(vec3({0.0f, 1.0f, 0.0f})) {};

		void	setOrthographicProjection(float left, float right, float top, float bottom, float near, float far);
		void	setPerspectiveProjection(float fovy, float aspect, float near, float far);
		void	setViewMatrix( void ) noexcept;

		const mat4&	getProjectionMatrix( bool recalculate = false ) noexcept;
		const mat4&	getViewMatrix( bool recalculate = true) noexcept;

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

		vec3	_position;		// position of the camera  NB |SCOP_CAMERA_DISTANCE| = 10 Config
		vec3	_forward;		// where the camera is pointing
		vec3	__up;			// general up
		vec3	_cameraForward;	// z axis of the camera
		vec3	_cameraLeft;	// x axis of the camera
		vec3	_cameraUp;		// y axis of the camera
};

} // namespace ve