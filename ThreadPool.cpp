#include "ThreadPool.h"
#include <thread>


ThreadPool::ThreadPool() 
{

}

void ThreadPool::init(int threadNum/* = 6*/)
{
	for (int i = 0; i < threadNum; ++i)
	{
		auto t = std::make_shared<std::thread>(threadFunc,this);
		//减少拷贝
		m_thread.push_back(std::move(t));
	}
}

void ThreadPool::destory()
{
	m_exitFlag = true;

	for (int i = 0; i < m_thread.size(); ++i)
	{
		m_thread[i]->join();
	}
}

// void ThreadPool::addTask(Task* task)
// {
// 	std::lock_guard<std::mutex> scopeLock(m_taskMutex);
// 	m_taskQueue.push_back(task);
// }

void ThreadPool::addTask(Task* task)
{
	std::lock_guard<std::mutex> scopeLock(m_taskMutex);
	m_taskQueue.push_back(task);
	m_taskCV.notify_one();
}


// void ThreadPool::threadFunc(void* threadParam) {

// 	Task* pTask = NULL;
// 	ThreadPool* pThreadPool = static_cast<ThreadPool*>(threadParam);

// 	while (!pThreadPool->m_exitFlag)
// 	{
// 		//pThreadPool->m_taskMutex.lock();换成lock_guard并减少锁的粒度
// 		{
// 			std::lock_guard<std::mutex> scopeLock(pThreadPool->m_taskMutex);

// 			if (!pThreadPool->m_taskQueue.empty())
// 			{
// 				pTask = *pThreadPool->m_taskQueue.begin();
// 			}
// 		}
		
// 		//pThreadPool->m_taskMutex.unlock();

// 		if (pTask != NULL)
// 		{
// 			pTask->doIt();
// 		}
// 	}
// }

//使用条件变量
void ThreadPool::threadFunc(void* threadParam) {

	Task* pTask = NULL;
	ThreadPool* pThreadPool = static_cast<ThreadPool*>(threadParam);

	while (!pThreadPool->m_exitFlag)
	{
		{
			std::unique_lock<std::mutex> lock(pThreadPool->m_taskMutex);

			while (pThreadPool->m_taskQueue.empty())
			{
				pThreadPool->m_taskCV.wait(lock);
			}

			auto iter = pThreadPool->m_taskQueue.begin();
			pTask = *iter;
			pThreadPool->m_taskQueue.erase(iter);
		}

		if (pTask != NULL)
		{
			pTask->doIt();
		}
	}
}
