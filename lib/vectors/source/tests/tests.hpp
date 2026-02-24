#pragma once

#include <glm/glm.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>

#include "Vectors.hpp"

#include <iostream>
#include <iomanip>

#define IMPRECISION_TOLERANCE 0.001f

#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define RESET	"\e[m"

int	runMatrixTests();
int	runVectorTests();
int	runQuaternionTests();
int	compareSizes(size_t a, size_t b);

template<typename T>
void	print3x3(const T& matrix)
{
	for (int row = 0; row < 3; row++)
	{
		std::cout << "[ ";
		for (int col = 0; col < 3; col++)
		{
			std::cout << std::setw(5) << std::setprecision(3) << matrix[row][col] << " ";
		}
		std::cout << "]\n";
	}
}

template<typename T>
void	print4x4(const T& matrix)
{
	for (int row = 0; row < 4; row++)
	{
		std::cout << "[ ";
		for (int col = 0; col < 4; col++)
		{
			std::cout << std::setw(5) << std::setprecision(3) << matrix[row][col] << " ";
		}
		std::cout << "]\n";
	}
}
