#include "Vox.hpp"

#include <iostream>
#include <stdexcept>
#include <thread>

int	main( void )
{
	vox::Vox app;
	try
	{
		app.run();
	}
	catch (const std::exception& e)
	{
		std::cerr << "Error: " << e.what() << std::endl;
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}
