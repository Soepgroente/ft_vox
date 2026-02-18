#include "Vox.hpp"

#include <cstdlib>
#include <iostream>
#include <stdexcept>


int	main( void )
{
	vox::Vox app;
	if (app.initialize() == false)
	{
		std::cerr << "Failed to initialize application" << std::endl;
		return 1;
	}
	try
	{
		app.run();
		app.shutdown();
	}
	catch (const std::exception& e)
	{
		std::cerr << "Error: " << e.what() << std::endl;
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}
