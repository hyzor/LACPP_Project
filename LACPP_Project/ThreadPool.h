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

inline ThreadPool::ThreadPool(unsigned int size)
	: mSize(size), mStop(false)
{
	for (unsigned int i = 0; i < mSize; ++i)
	{
		mThreads.emplace_back(
				[this]
				 {
					for (;;)
					{
						std::function<void()> task;

						{
							std::unique_lock<std::mutex> lock(this->mQueueMutex);
							this->mCondition.wait(lock,
								[this] { return this->mStop || !this->mTasks.empty(); });

							if (this->mStop && this->mTasks.empty())
								return;

							task = std::move(this->mTasks.front());
							this->mTasks.pop();
						}

						task();
					}
				 }
				 );
	}
}

template<class Func, class... Args>
auto ThreadPool::AddTask(Func&& func, Args&&... args)
	-> std::future<typename std::result_of<Func(Args...)>::type>
{
	using return_type = typename std::result_of<Func(Args...)>::type;

	auto task = std::make_shared<std::packaged_task<return_type()>>(
			std::bind(std::forward<Func>(func), std::forward<Args>(args)...));

	std::future<return_type> result = task->get_future();

	{
		std::unique_lock<std::mutex> lock(mQueueMutex);

		if (mStop)
		{
			throw std::runtime_error("ThreadPool is stopped, cannot add task!");
		}

		mTasks.emplace([task]() { (*task)(); });
	}

	mCondition.notify_one();
	return result;
}

inline ThreadPool::~ThreadPool()
{
	//Shutdown();
}

inline void ThreadPool::Shutdown()
{
	{
		std::unique_lock<std::mutex> lock(mQueueMutex);
		mStop = true;
	}

	mCondition.notify_all();

	for (unsigned int i = 0; i < mThreads.size(); ++i)
	{
		mThreads[i].join();
	}
}

#endif /* THREADPOOL_H_ */
