#include "Vox.hpp"

#include <cstdlib>
#include <iostream>
#include <stdexcept>


int	main(int argc, char** argv)
{
	if (argc != 2)		// NB implement argParser
	{
		std::cerr << "Usage: " << argv[0] << " <path_to_obj_file>" << std::endl;
		return 1;
	}

	vox::Vox app(argv[1]);
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
