#include "MouseInput.hpp"


namespace vox {

/*
 * Zero all data
 */
void	MouseInput::reset() noexcept
{
	mouseStates.fill(false);
	buttonsPressed.fill(false);
	buttonsReleased.fill(false);
}

/*
 * Update the cursor position
 *
 * @param posX new cursor x position (relative to the monitor: (0;0) -> top-left corner)
 *
 * @param posY new cursor y position (relative to the monitor: (0;0) -> top-left corner)
 */
void	MouseInput::setCursorPos( float posX, float posY ) noexcept {
	this->posX = posX;
	this->posY = posY;
}

/*
 * Get the cursor position
 *
 * @param posX will store the current x cursor position (relative 
 * to the monitor: (0;0) -> top-left corner)
 *
 * @param posY will store the current y cursor position (relative 
 * to the monitor: (0;0) -> top-left corner)
 */
void	MouseInput::getCursorPos( float& posX, float& posY) const noexcept {
	posX = this->posX;
	posY = this->posY;
}

}	// namespace vox
