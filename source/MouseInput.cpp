#include "MouseInput.hpp"


namespace vox {

void	MouseInput::reset() noexcept
{
	mouseStates.fill(false);
	buttonsPressed.fill(false);
	buttonsReleased.fill(false);
}

void	MouseInput::setCursorPos( float posX, float posY ) noexcept {
	this->posX = posX;
	this->posY = posY;
}

void	MouseInput::getCursorPos( float& posX, float& posY) noexcept {
	posX = this->posX;
	posY = this->posY;
}

}	// namespace vox
