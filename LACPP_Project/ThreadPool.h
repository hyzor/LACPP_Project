// File: ThreadPool.h
//*****************************************************************************************
// This file contains an implementation of a thread pool that is
// heavily reliant on C++11 features.
// Most of the code is authored by Jacob Progsch and can be found at:
// https://github.com/progschj/ThreadPool
//*****************************************************************************************

#ifndef THREADPOOL_H_
#define THREADPOOL_H_

#include <mutex>
#include <thread>
#include <condition_variable>
#include <queue>
#include <vector>
#include <future>

#define DEFAULT_POOL_SIZE 10

class ThreadPool
{
public:
	ThreadPool();
	ThreadPool(unsigned int size);
	~ThreadPool();

	template<class Func, class... Args>
	auto AddTask(Func&& func, Args&&... args)
		-> std::future<typename std::result_of<Func(Args...)>::type>;

	void Init();
	void Shutdown();
private:
	std::vector<std::thread> mThreads;
	std::queue<std::function<void()>> mTasks;
	std::mutex mQueueMutex;
	std::condition_variable mCondition;
	unsigned int mSize;
	bool mStop;
};

inline ThreadPool::ThreadPool()
	: mSize(DEFAULT_POOL_SIZE), mStop(false)
{
	ThreadPool(mSize);
}

// Adds some number of threads to our pool
inline ThreadPool::ThreadPool(unsigned int size)
	: mSize(size), mStop(false)
{
	for (unsigned int i = 0; i < mSize; ++i)
	{
		mThreads.emplace_back(
				[this]
				 {

					// Each thread should just be continuously looping,
					// and try to pick a task when available
					for (;;)
					{
						std::function<void()> task;

						{
							// Acquire exclusive access to our task queue
							std::unique_lock<std::mutex> lock(this->mQueueMutex);

							// Wait until thread pool responds
							this->mCondition.wait(lock,
								[this] { return this->mStop || !this->mTasks.empty(); });

							// If the thread pool has stopped and there are no tasks
							// available, abort
							if (this->mStop && this->mTasks.empty())
								return;

							// Task found, set it to this thread and pop it from
							// the task list
							task = std::move(this->mTasks.front());
							this->mTasks.pop();
						}

						task();
					}
				 }
				 );
	}
}

// Adds a new task at the end of the queue. The function is a lambda function
// where the return type is determined at run-time
template<class Func, class... Args>
auto ThreadPool::AddTask(Func&& func, Args&&... args)
	-> std::future<typename std::result_of<Func(Args...)>::type>
{
	using return_type = typename std::result_of<Func(Args...)>::type;

	// Wrap the function together with the arguments so that we may invoke it
	// asynchronously and make a shared_ptr of it
	auto task = std::make_shared<std::packaged_task<return_type()>>(
			std::bind(std::forward<Func>(func), std::forward<Args>(args)...));

	std::future<return_type> result = task->get_future();

	{
		// Acquire exclusive access to our task queue
		std::unique_lock<std::mutex> lock(mQueueMutex);

		// We don't want to be able to add tasks if the thread pool has stopped
		if (mStop)
		{
			throw std::runtime_error("ThreadPool is stopped, cannot add task!");
		}

		// Now place the new task at the end of our task queue
		mTasks.emplace([task]() { (*task)(); });
	}

	// Notify one waiting thread that we have a new task available,
	// by unblocking the thread
	mCondition.notify_one();

	return result;
}

inline ThreadPool::~ThreadPool()
{
	//Shutdown();
}

// Call this before destroying the thread pool object.
// Notifies all threads that we are stopping and then waits for them to finish.
inline void ThreadPool::Shutdown()
{
	{
		std::unique_lock<std::mutex> lock(mQueueMutex);
		mStop = true;
	}

	// Notify all threads that we have stopped, by unblocking all of them
	mCondition.notify_all();

	// Now wait for all the threads to finish
	for (unsigned int i = 0; i < mThreads.size(); ++i)
	{
		mThreads[i].join();
	}
}

#endif /* THREADPOOL_H_ */
