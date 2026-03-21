#pragma once

#include <condition_variable>
#include <cstddef>
#include <future>
#include <mutex>
#include <queue>
#include <thread>
#include <type_traits>
#include <utility>
#include <vector>
#include <functional>


class ThreadManager
{
	public:
		ThreadManager();
		~ThreadManager() noexcept;
		
		ThreadManager(const ThreadManager&) = delete;
		ThreadManager(ThreadManager&&) = delete;
		ThreadManager& operator=(const ThreadManager&) = delete;
		ThreadManager& operator=(ThreadManager&&) = delete;
		
		void	joinWorkerThreads();
		void	stop();
		void	waitIdle();

		/*	Adds a task to the pool and activates an idle thread to execute it. Result is stored */
		template <class F>
		auto enqueue(F&& function) -> std::future<std::invoke_result_t<F>>
		{
			using R = std::invoke_result_t<F>;
		
			std::shared_ptr	task = std::make_shared<std::packaged_task<R()>>(std::forward<F>(function));
			std::future<R>	fut = task->get_future();
		
			{
				std::lock_guard<std::mutex> lock(mutex);

				if (shouldRun == false)
				{
					throw std::runtime_error("enqueue on stopped ThreadPool");
				}
				jobs.emplace([task] { (*task)(); });
			}
			cv.notify_one();
			return fut;
		}

		private:
		
		void	workerLoop();
		
		std::condition_variable	cv;
		std::condition_variable	idleCv;
		std::vector<std::thread>	workerThreads;
		std::queue<std::function<void()>> jobs;
		std::mutex	mutex;

		bool	shouldRun;
		int		activeWorkers;
};