#include "tests.hpp"

int	compareSizes(size_t a, size_t b)
{
	if (a == b)
	{
		std::cout << GREEN << "[OK]\n" << RESET;
		return 0;
	}
	else
	{
		std::cout << RED << "[FAIL]\n" << RESET;
		std::cout << "Expected: " << b;
		std::cout << ", Got: " << a;
		std::cout << std::endl;
		return 1;
	}
}
