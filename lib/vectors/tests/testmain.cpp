#include "tests.hpp"

int main()
{
	int	results = 0;

	results += runMatrixTests();
	results += runVectorTests();
	results += runQuaternionTests();

	std::cout << "Total errors: " << results << "\n";
	return results;
}
