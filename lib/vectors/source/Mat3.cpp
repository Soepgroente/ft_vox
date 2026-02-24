#include "Mat3.hpp"
#include "Vec3.hpp"
#include <cstring>

mat3::mat3(float diagonal) : data{{0.0f}, {0.0f}, {0.0f}}
{
	data[0][0] = diagonal;
	data[1][1] = diagonal;
	data[2][2] = diagonal;
}

mat3::mat3(const vec3& row0, const vec3& row1, const vec3& row2)
{
	data[0][0] = row0.x; data[0][1] = row0.y; data[0][2] = row0.z;
	data[1][0] = row1.x; data[1][1] = row1.y; data[1][2] = row1.z;
	data[2][0] = row2.x; data[2][1] = row2.y; data[2][2] = row2.z;
}

mat3::mat3(std::initializer_list<std::initializer_list<float>> rows)
{
	int i = 0;
	for (const std::initializer_list<float>& row : rows)
	{
		int j = 0;
		for (float val : row)
		{
			if (i < 3 && j < 3)
			{
				data[j][i] = val;
			}
			j++;
		}
		i++;
	}
}

mat3	mat3::operator*(const mat3& other) const
{
	mat3	result(0.0f);
	
	for (int col = 0; col < 3; col++)
	{
		for (int row = 0; row < 3; row++)
		{
			for (int k = 0; k < 3; k++)
			{
				result.data[row][col] += data[k][col] * other.data[row][k];
			}
		}
	}
	return result;
}

std::ostream&	operator<<(std::ostream& os, const mat3& matrix)
{
	for (int row = 0; row < 3; row++)
	{
		os << "[";
		for (int col = 0; col < 3; col++)
		{
			os << matrix.data[row][col] << "]";
			if (col < 2)
			{
				os << " [";
			}
		}
		os << "\n";
	}
	return os;
}