#include "MouseInput.hpp"

namespace vox {

MouseInput::MouseInput()
{
	reset();
}

void	MouseInput::reset()
{
	mouseStates.fill(false);
	buttonsPressed.fill(false);
	buttonsReleased.fill(false);
}

void	MouseInput::setCursorPos( float posX, float posY ) {
	this->posX = posX;
	this->posY = posY;
}

void	MouseInput::getCursorPos( float& posX, float& posY) {
	posX = this->posX;
	posY = this->posY;
}

}	// namespace vox