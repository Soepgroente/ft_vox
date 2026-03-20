#pragma once

#include <chrono>

class Stopwatch
{
	public:
		Stopwatch() noexcept : startTime(std::chrono::high_resolution_clock::now()) {};
		~Stopwatch() noexcept = default;
		Stopwatch(const Stopwatch& other) = delete;
		Stopwatch(Stopwatch&& other) = delete;
		Stopwatch& operator=(const Stopwatch& other) = delete;
		Stopwatch& operator=(Stopwatch&& other) = delete;

		void	start() noexcept { this->startTime = std::chrono::high_resolution_clock::now(); }
		void	elapsed(std::ostream& stream) const noexcept;
		void	stop() noexcept { this->endTime = std::chrono::high_resolution_clock::now(); }

	private:
		std::chrono::high_resolution_clock::time_point startTime;
		std::chrono::high_resolution_clock::time_point endTime;
};

std::ostream&	operator<<(std::ostream& os, const Stopwatch& stopwatch);