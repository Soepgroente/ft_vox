#include "ThreadManager.hpp"
#include <iostream>

ThreadManager::ThreadManager() : shouldRun(true), activeWorkers(0)
{
	int workers = std::max(std::thread::hardware_concurrency() - 1, 1U);

	std::cout << "Created " << workers << " worker threads" << std::endl;
	workerThreads.reserve(workers);
	for (int i = 0; i < workers; i++)
	{
		workerThreads.emplace_back(&ThreadManager::workerLoop, this);
	}
}

ThreadManager::~ThreadManager() noexcept
{
	joinWorkerThreads();
}

void ThreadManager::joinWorkerThreads()
{
	for (std::thread& workerThread : workerThreads)
	{
		if (workerThread.joinable() == true)
		{
			workerThread.join();
		}
	}
	workerThreads.clear();
}

/*	All worker threads are in this loop, waiting to be notified in cv.wait.
	If a thread gets activated, it takes the first job in the queue and executes it. 
	After finishing, it checks if there are still jobs to do, and if not, it notifies the main thread that is waiting for idle state in idleCv.wait.
*/

void	ThreadManager::workerLoop()
{
	while (true)
	{
		std::function<void()> job;
		{
			std::unique_lock<std::mutex> lock(mutex);

			cv.wait(lock, [this] { return shouldRun == false || jobs.empty() == false; });

			if (shouldRun == false && jobs.empty())
			{
				return;
			}

			job = std::move(jobs.front());
			jobs.pop();
			activeWorkers++;
		}

		job();

		{
			std::lock_guard<std::mutex> lock(mutex);

			activeWorkers--;
			if (jobs.empty() == true && activeWorkers == 0)
			{
				idleCv.notify_all();
			}
		}
	}
}

void ThreadManager::stop()
{
	{
		std::lock_guard<std::mutex> lock(mutex);
		shouldRun = false;
	}
	cv.notify_all();
	joinWorkerThreads();
}

void	ThreadManager::waitIdle()
{
	std::unique_lock<std::mutex>	lock(mutex);

	idleCv.wait(lock, [this] { return jobs.empty() == true && activeWorkers == 0; });
}


