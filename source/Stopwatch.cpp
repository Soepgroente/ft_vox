#include "Stopwatch.hpp"
#include <iostream>

using ui64 = uint64_t;

float	Stopwatch::elapsed(Unit type) const noexcept
{
	switch (type)
	{
		case Nanoseconds:
			return ns();
		case Microseconds:
			return us();
		case Milliseconds:
			return ms();
		case Seconds:
			return s();
		default:
			return 0.0f;
	}
}

void	Stopwatch::reset() noexcept
{
	startTime = now();
	endTime = startTime;
	elapsedTime = Duration::zero();
}

std::ostream&	operator<<(std::ostream& os, const Stopwatch& stopwatch)
{
	Duration elapsed = stopwatch.elapsed();
	std::__1::chrono::steady_clock::rep elapsedTime = elapsed.count();

	if (elapsedTime < 1000)
	{
		os << "Elapsed time: " << stopwatch.elapsed(Nanoseconds) << " nanoseconds";
	}
	else if (elapsedTime < 1000000)
	{
		os << "Elapsed time: " << stopwatch.elapsed(Microseconds) << " microseconds";
	}
	else if (elapsedTime < 1000000000)
	{
		os << "Elapsed time: " << stopwatch.elapsed(Milliseconds) << " milliseconds";
	}
	else
	{
		os << "Elapsed time: " << stopwatch.elapsed(Seconds) << " seconds";
	}
	os << std::endl;
	return os;
}