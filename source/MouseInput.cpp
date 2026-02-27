#include "MouseInput.hpp"

namespace vox {

MouseInput::MouseInput()
{
	this->posX = 0.0f;
	this->posY = 0.0f;
	this->lastPosX = 0.0f;
	this->lastPosY = 0.0f;
	reset();
}

void	MouseInput::reset()
{
	mouseStates.fill(false);
	buttonsPressed.fill(false);
	buttonsReleased.fill(false);
}

void	MouseInput::setCursorPos( float posX, float posY ) {
	this->lastPosX = this->posX;
	this->lastPosY = this->posY;
	this->posX = posX;
	this->posY = posY;
}

void	MouseInput::getCursorPos( float& posX, float& posY) {
	posX = this->posX;
	posY = this->posY;
}

bool	MouseInput::cursorPositionHasChanged( float& deltaX, float& deltaY ) noexcept {
	deltaX = this->posX - this->lastPosX;
	deltaY = this->posY - this->lastPosY;
	this->lastPosX = this->posX;
	this->lastPosY = this->posY;

	return std::fabs(deltaX) > epsilon() or std::fabs(deltaY) > epsilon();
}

}	// namespace vox