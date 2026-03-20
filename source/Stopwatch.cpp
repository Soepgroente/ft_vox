#include "Stopwatch.hpp"
#include <iostream>

using ui64 = uint64_t;

void	Stopwatch::elapsed(std::ostream& stream) const noexcept
{
	ui64 elapsedTime = std::chrono::duration_cast<std::chrono::microseconds>(this->endTime - this->startTime).count();

	if (elapsedTime < 1000)
	{
		stream << "Elapsed time: " << elapsedTime << " microseconds" << std::endl;
	}
	else if (elapsedTime < 1000000)
	{
		stream << "Elapsed time: " << elapsedTime / 1000.0f << " milliseconds" << std::endl;
	}
	else
	{
		stream << "Elapsed time: " << elapsedTime / 1000000.0f << " seconds" << std::endl;
	}
}

std::ostream&	operator<<(std::ostream& os, const Stopwatch& stopwatch)
{
	stopwatch.elapsed(os);
	return os;
}