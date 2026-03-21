#pragma once

#include <chrono>

using Clock = std::chrono::steady_clock;
using Time = Clock::time_point;
using Duration = Clock::duration;

enum Unit
{
	Nanoseconds,
	Microseconds,
	Milliseconds,
	Seconds
};

class Stopwatch
{
	public:
		Stopwatch() noexcept : startTime(now()) {};
		~Stopwatch() noexcept = default;
		Stopwatch(const Stopwatch& other) = delete;
		Stopwatch(Stopwatch&& other) = delete;
		Stopwatch& operator=(const Stopwatch& other) = delete;
		Stopwatch& operator=(Stopwatch&& other) = delete;

		void	start() noexcept { startTime = now(); }
		void	stop() noexcept { endTime = now(); elapsedTime = endTime - startTime; }
		void	reset() noexcept;

		Duration	elapsed() const noexcept { return elapsedTime; }
		float		elapsed(Unit type) const noexcept;
		
	private:
		Time	startTime;
		Time	endTime;
		Duration	elapsedTime;
		
		Time	now() const noexcept { return std::chrono::high_resolution_clock::now(); }
		float	ns() const noexcept { return static_cast<float>(elapsedTime.count()); }
		float	us() const noexcept { return static_cast<float>(elapsedTime.count()) / 1000.0f; }
		float	ms() const noexcept { return static_cast<float>(elapsedTime.count()) / 1000000.0f; }
		float	s() const noexcept { return static_cast<float>(elapsedTime.count()) / 1000000000.0f; }
};

std::ostream&	operator<<(std::ostream& os, const Stopwatch& stopwatch);