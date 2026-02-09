#include "Vox.hpp"

int	main()
{
	vox::Vox	app;
	
	if (app.initialize() == false)
	{
		std::cerr << "Failed to initialize application" << std::endl;
		return 1;
	}

	app.run();
	app.shutdown();
	
	return 0;
}
