#include "tests.hpp"

static void	print3x3TestResult(int passed, const glm::mat3& glmMat, const mat3& myMat)
{
	if (passed == 0)
	{
		std::cout << GREEN << "[OK] " << RESET << "\n";
	}
	else
	{
		std::cout << RED << "[FAIL] " << RESET << "\n";
		std::cout << "Expected:\n";
		print3x3(glmMat);
		std::cout << "Got:\n";
		print3x3(myMat);
	}
}

static void	print4x4TestResult(int passed, const glm::mat4& glmMat, const mat4& myMat)
{
	if (passed == 0)
	{
		std::cout << GREEN << "[OK] " << RESET << "\n";
	}
	else
	{
		std::cout << RED << "[FAIL] " << RESET << "\n";
		std::cout << "Expected:\n";
		print4x4(glmMat);
		std::cout << "Got:\n";
		print4x4(myMat);
	}
}

/* static bool	compareVec3(const vec3& myVec, const glm::vec3& glmVec)
{
	bool	result = true;

	if (std::abs(myVec.x - glmVec.x) > IMPRECISION_TOLERANCE ||
		std::abs(myVec.y - glmVec.y) > IMPRECISION_TOLERANCE ||
		std::abs(myVec.z - glmVec.z) > IMPRECISION_TOLERANCE)
	{
		result = false;
	}
	return result;
}
*/
static int	compareMat4(const mat4& myMat, const glm::mat4& glmMat)
{
	for (int row = 0; row < 4; row++)
	{
		for (int col = 0; col < 4; col++)
		{
			if (std::abs(myMat.data[row][col] - glmMat[row][col]) > IMPRECISION_TOLERANCE)
			{
				return 1;
			}
		}
	}
	return 0;
}

static int	compareMat3(const mat3& myMat, const glm::mat3& glmMat)
{
	for (int row = 0; row < 3; row++)
	{
		for (int col = 0; col < 3; col++)
		{
			if (std::abs(myMat[row][col] - glmMat[row][col]) > IMPRECISION_TOLERANCE)
			{
				return 1;
			}
		}
	}
	return 0;
}

static int	mat4InitializerTests()
{
	std::cout << "Matrix initializers:\n";

	int		failures = 0;
	int		result;
	void	(*printFunction)(int, const glm::mat4&, const mat4&) = print4x4TestResult;

	{
		mat4		myMat{};
		glm::mat4	glmMat{};

		result = compareMat4(myMat, glmMat);
		failures += result;
		printFunction(result, glmMat, myMat);
	}
	{
		mat4		myMat(2.0f);
		glm::mat4	glmMat(2.0f);

		result = compareMat4(myMat, glmMat);
		failures += result;
		printFunction(result, glmMat, myMat);
	}
	{
		mat4	myMat(
			{1.0f, 2.0f, 3.0f, 4.0f},
			{5.0f, 6.0f, 7.0f, 8.0f},
			{9.0f, 10.0f, 11.0f, 12.0f},
			{13.0f, 14.0f, 15.0f, 16.0f});
		glm::mat4	glmMat(
			{1.0f, 2.0f, 3.0f, 4.0f},
			{5.0f, 6.0f, 7.0f, 8.0f},
			{9.0f, 10.0f, 11.0f, 12.0f},
			{13.0f, 14.0f, 15.0f, 16.0f}
		);

		result = compareMat4(myMat, glmMat);
		failures += result;
		printFunction(result, glmMat, myMat);
	}
		{
		mat4	myMat{
			{1.0f, 2.0f, 3.0f, 4.0f},
			{5.0f, 6.0f, 7.0f, 8.0f},
			{9.0f, 10.0f, 11.0f, 12.0f},
			{13.0f, 14.0f, 15.0f, 16.0f}
		};
		glm::mat4	glmMat{
			{1.0f, 2.0f, 3.0f, 4.0f},
			{5.0f, 6.0f, 7.0f, 8.0f},
			{9.0f, 10.0f, 11.0f, 12.0f},
			{13.0f, 14.0f, 15.0f, 16.0f}
		};

		result = compareMat4(myMat, glmMat);
		failures += result;
		printFunction(result, glmMat, myMat);
	}
	{
		mat4	myMat{
			{1.0f, 0.0f, 0.0f, 0.0f},
			{0.0f, 1.0f, 0.0f, 0.0f},
			{0.0f, 0.0f, 1.0f, 0.0f},
			{0.0f, 0.0f, 0.0f, 1.0f}
		};
		glm::mat4	glmMat{
			{1.0f, 0.0f, 0.0f, 0.0f},
			{0.0f, 1.0f, 0.0f, 0.0f},
			{0.0f, 0.0f, 1.0f, 0.0f},
			{0.0f, 0.0f, 0.0f, 1.0f}
		};

		result = compareMat4(myMat, glmMat);
		failures += result;
		printFunction(result, glmMat, myMat);
	}
	{
		// Test a translation matrix
		mat4	myMat{
			{1.0f, 0.0f, 0.0f, 5.0f},
			{0.0f, 1.0f, 0.0f, 10.0f},
			{0.0f, 0.0f, 1.0f, 15.0f},
			{0.0f, 0.0f, 0.0f, 1.0f}
		};
		glm::mat4	glmMat{
			{1.0f, 0.0f, 0.0f, 5.0f},
			{0.0f, 1.0f, 0.0f, 10.0f},
			{0.0f, 0.0f, 1.0f, 15.0f},
			{0.0f, 0.0f, 0.0f, 1.0f}
		};

		result = compareMat4(myMat, glmMat);
		failures += result;
		printFunction(result, glmMat, myMat);
	}
	return failures;
}

static int	mat4MultiplicationTests()
{
	std::cout << "Matrix multiplication:\n";

	int		failures = 0;
	bool	result;
	void	(*printFunction)(int, const glm::mat4&, const mat4&) = print4x4TestResult;

	{
		mat4		myMatA(
			{1.0f, 2.0f, 3.0f, 4.0f},
			{5.0f, 6.0f, 7.0f, 8.0f},
			{9.0f, 10.0f, 11.0f, 12.0f},
			{13.0f, 14.0f, 15.0f, 16.0f}
		);
		mat4		myMatB(
			{16.0f, 15.0f, 14.0f, 13.0f},
			{12.0f, 11.0f, 10.0f, 9.0f},
			{8.0f, 7.0f, 6.0f, 5.0f},
			{4.0f, 3.0f, 2.0f, 1.0f}
		);
		glm::mat4	glmMatA(
			{1.0f, 2.0f, 3.0f, 4.0f},
			{5.0f, 6.0f, 7.0f, 8.0f},
			{9.0f, 10.0f, 11.0f, 12.0f},
			{13.0f, 14.0f, 15.0f, 16.0f}
		);
		glm::mat4	glmMatB(
			{16.0f, 15.0f, 14.0f, 13.0f},
			{12.0f, 11.0f, 10.0f, 9.0f},
			{8.0f, 7.0f, 6.0f, 5.0f},
			{4.0f, 3.0f, 2.0f, 1.0f}
		);

		mat4		myResult = myMatA * myMatB;
		glm::mat4	glmResult = glmMatA * glmMatB;
		result = compareMat4(myResult, glmResult);
		failures += result;
		printFunction(result, glmResult, myResult);
	}
	{
		mat4		myMatA(
			{1.0f, 0.0f, 0.0f, 0.0f},
			{0.0f, 1.0f, 0.0f, 0.0f},
			{0.0f, 0.0f, 1.0f, 0.0f},
			{0.0f, 0.0f, 0.0f, 1.0f}
		);
		mat4		myMatB(
			{5.0f, 6.0f, 7.0f, 8.0f},
			{9.0f, 10.0f, 11.0f, 12.0f},
			{13.0f, 14.0f, 15.0f, 16.0f},
			{17.0f, 18.0f, 19.0f, 20.0f}
		);
		glm::mat4	glmMatA(
			{1.0f, 0.0f, 0.0f, 0.0f},
			{0.0f, 1.0f, 0.0f, 0.0f},
			{0.0f, 0.0f, 1.0f, 0.0f},
			{0.0f, 0.0f, 0.0f, 1.0f}
		);
		glm::mat4	glmMatB(
			{5.0f, 6.0f, 7.0f, 8.0f},
			{9.0f, 10.0f, 11.0f, 12.0f},
			{13.0f, 14.0f, 15.0f, 16.0f},
			{17.0f, 18.0f, 19.0f, 20.0f}
		);

		mat4		myResult = myMatA * myMatB;
		glm::mat4	glmResult = glmMatA * glmMatB;
		result = compareMat4(myResult, glmResult);
		failures += result;
		printFunction(result, glmResult, myResult);
	}
	return failures;
}

static int	mat4TranslationTests()
{
	std::cout << "Matrix translation:\n";

	int		failures = 0;
	bool	result;
	void	(*printFunction)(int, const glm::mat4&, const mat4&) = print4x4TestResult;

	{
		mat4		myMat{};
		glm::mat4	glmMat{};
		vec3		translation(3.0f, 4.0f, 5.0f);
		glm::vec3	glmTranslation(3.0f, 4.0f, 5.0f);

		myMat.translate(translation);
		glmMat = glm::translate(glmMat, glmTranslation);

		result = compareMat4(myMat, glmMat);
		failures += result;
		printFunction(result, glmMat, myMat);
	}
	{
		mat4		myMat(
			{1.0f, 2.0f, 3.0f, 4.0f},
			{5.0f, 6.0f, 7.0f, 8.0f},
			{9.0f, 10.0f, 11.0f, 12.0f},
			{13.0f, 14.0f, 15.0f, 16.0f}
		);
		glm::mat4	glmMat(
			{1.0f, 2.0f, 3.0f, 4.0f},
			{5.0f, 6.0f, 7.0f, 8.0f},
			{9.0f, 10.0f, 11.0f, 12.0f},
			{13.0f, 14.0f, 15.0f, 16.0f}
		);
		vec3		translation(-2.0f, -4.0f, -6.0f);
		glm::vec3	glmTranslation(-2.0f, -4.0f, -6.0f);

		myMat.translate(translation);
		glmMat = glm::translate(glmMat, glmTranslation);

		result = compareMat4(myMat, glmMat);
		failures += result;
		printFunction(result, glmMat, myMat);
	}
	return failures;
}

static int mat3InitializerTests()
{
	std::cout << "Matrix initializers:\n";

	int		failures = 0;
	bool	result;
	void	(*printFunction)(int, const glm::mat3&, const mat3&) = print3x3TestResult;

	{
		mat3		myMat{};
		glm::mat3	glmMat{};

		result = compareMat3(myMat, glmMat);
		failures += result;
		printFunction(result, glmMat, myMat);
	}
	{
		mat3		myMat(2.0f);
		glm::mat3	glmMat(2.0f);

		result = compareMat3(myMat, glmMat);
		failures += result;
		printFunction(result, glmMat, myMat);
	}
	{
		mat3	myMat(
			{1.0f, 2.0f, 3.0f},
			{4.0f, 5.0f, 6.0f},
			{7.0f, 8.0f, 9.0f});
		glm::mat3	glmMat(
			{1.0f, 2.0f, 3.0f},
			{4.0f, 5.0f, 6.0f},
			{7.0f, 8.0f, 9.0f}
		);

		result = compareMat3(myMat, glmMat);
		failures += result;
		printFunction(result, glmMat, myMat);
	}
	return failures;
}

static int	mat3MultiplicationTests()
{
	std::cout << "Matrix multiplication:\n";

	int		failures = 0;
	bool	result;
	void	(*printFunction)(int, const glm::mat3&, const mat3&) = print3x3TestResult;

	{
		mat3		myMatA(
			{1.0f, 2.0f, 3.0f},
			{4.0f, 5.0f, 6.0f},
			{7.0f, 8.0f, 9.0f}
		);
		mat3		myMatB(
			{9.0f, 8.0f, 7.0f},
			{6.0f, 5.0f, 4.0f},
			{3.0f, 2.0f, 1.0f}
		);
		glm::mat3	glmMatA(
			{1.0f, 2.0f, 3.0f},
			{4.0f, 5.0f, 6.0f},
			{7.0f, 8.0f, 9.0f}
		);
		glm::mat3	glmMatB(
			{9.0f, 8.0f, 7.0f},
			{6.0f, 5.0f, 4.0f},
			{3.0f, 2.0f, 1.0f}
		);

		mat3		myResult = myMatA * myMatB;
		glm::mat3	glmResult = glmMatA * glmMatB;
		result = compareMat3(myResult, glmResult);
		failures += result;
		printFunction(result, glmResult, myResult);
	}
	return failures;
}

int	mat3Tests()
{
	std::cout << "[3x3] ";

	int 	failures = 0;

	failures += mat3InitializerTests();
	failures += mat3MultiplicationTests();
	return failures;
}

int	mat4Tests()
{
	std::cout << "[4x4] ";

	int 	failures = 0;
	
	failures += mat4InitializerTests();
	failures += mat4MultiplicationTests();
	failures += mat4TranslationTests();
	return failures;
}

int	runMatrixTests()
{
	int	failures = 0;

	std::cout << "Matrix data size tests:\n";
	failures += compareSizes(sizeof(mat3), sizeof(glm::mat3));
	failures += compareSizes(sizeof(mat4), sizeof(glm::mat4));
	failures += mat3Tests();
	failures += mat4Tests();
	return failures;
}