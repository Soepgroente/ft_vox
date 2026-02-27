#include "KeyboardInput.hpp"

namespace vox {

KeyboardInput::KeyboardInput()
{
	reset();
	keysPressed.fill(false);
}

void	KeyboardInput::reset()
{
	keyStates.fill(false);
	// keysPressed.fill(false);
	keysReleased.fill(false);
}

}	// namespace vox