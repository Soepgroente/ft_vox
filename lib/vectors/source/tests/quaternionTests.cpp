#include "tests.hpp"

static bool	check(const vec3& a, const vec3& b)
{
	return	std::abs(a.x - b.x) < IMPRECISION_TOLERANCE &&
			std::abs(a.y - b.y) < IMPRECISION_TOLERANCE &&
			std::abs(a.z - b.z) < IMPRECISION_TOLERANCE;
}

static int	quatResult(const vec3& vector, const quat& quat, const vec3& expectedResult)
{
	vec3	resultingVector = quat::rotated(vector, quat);

	if (check(resultingVector, expectedResult) == true)
	{
		std::cout << GREEN << "[OK]\n" << RESET;
		return 0;
	}
	else
	{
		std::cout << RED << "[FAIL]\n" << RESET;
		std::cout << "Expected: " << expectedResult;
		std::cout << ", got: " << resultingVector;
		std::cout << std::endl;
	}
	return 1;
}


int	runQuaternionTests()
{
	int	failures = 0;
	std::cout << RESET << "Quaternion rotation tests:\n";
	failures += quatResult(vec3(0, 0, 1), quat(radians(-60), vec3(1, 0, 0)), vec3(0, 0.866, 0.5));
	failures += quatResult(vec3(0, 0, 1), quat(radians(660), vec3(1, 0, 0)), vec3(0, 0.866, 0.5));
	failures += quatResult(vec3(0, 0, 0), quat(radians(0), vec3(0, 0, 0)), vec3(0, 0, 0));
	failures += quatResult(vec3(1, 1, 1), quat(radians(0), vec3(0, 0, 0)), vec3(1, 1, 1));
	failures += quatResult(vec3(1, 1, 1), quat(radians(200), vec3(0, 1, 0)), vec3(-1.282, 1, -0.598));
	failures += quatResult(vec3(265, 2143, 1213), quat(radians(200), vec3(0, 1, 0)), vec3(-663.888978, 2143, -1049.211811));
	failures += quatResult(vec3(0, 0, 1), quat(radians(360), vec3(1, 0, 0)), vec3(0, 0, 1));
	failures += quatResult(vec3(0, 0, 1), quat(radians(720), vec3(1, 0, 0)), vec3(0, 0, 1));
	failures += quatResult(vec3(0, 0, 1), quat(radians(-180), vec3(1, 0, 0)), vec3(0, 0, -1));
	failures += quatResult(vec3(0, 0, 1), quat(radians(180), vec3(1, 0, 0)), vec3(0, 0, -1));
	failures += quatResult(vec3(0, 0, 0), quat(radians(10), vec3(1, 0, 0)), vec3(0, 0, 0));
	failures += quatResult(vec3(1, 1, 1), quat(radians(10), vec3(1, 0, 0)), vec3(1, 0.811, 1.158));
	return failures;
}