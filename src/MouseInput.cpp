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

}	// namespace vox