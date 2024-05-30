#include "ThreadPoolExecutor.h"
#include <thread>

ThreadPoolExcutor::ThreadPoolExcutor()
{

}

void ThreadPoolExcutor::init(int coreThreadNum, int maxThreadNum, int maxTaskNum)
{
	for (int i = 0; i < coreThreadNum; ++i)
	{
		auto t = std::make_shared<std::thread>(threadFunc, this);
		//减少拷贝
		m_coreThreads.push_back(std::move(t));
	}

	m_maxThreadNum = maxThreadNum;
	m_maxTaskNum = maxTaskNum;
}

void ThreadPoolExcutor::destory()
{
	m_exitFlag = true;

	for (int i = 0; i < m_coreThreads.size(); ++i)
	{
		m_coreThreads[i]->join();
	}
}

bool ThreadPoolExcutor::addTask(Task* task)
{
	std::lock_guard<std::mutex> scopeLock(m_taskMutex);

	if (m_taskQueue.size() > m_maxTaskNum)
	{
		return false;
	}

	m_taskQueue.push_back(task);
	if (m_runningTaskThreadNum > m_coreThreadNum && m_runningTaskThreadNum < m_maxThreadNum)
	{
		auto t = std::make_shared<std::thread>(threadFunc, this);
		m_extraThreads.push_back(std::move(t));
	}
	else if (m_runningTaskThreadNum > m_maxThreadNum)
	{
		return false;
	}

	m_taskCV.notify_one();

	return true;
}


//TODO:这里需要更新如果m_runningTaskThreadNum<=m_coreThreadNum,如何把extraThreads结束
void ThreadPoolExcutor::threadFunc(void* threadParam) {

	Task* pTask = NULL;
	ThreadPoolExcutor* pThreadPool = static_cast<ThreadPoolExcutor*>(threadParam);

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
			//m_runningTaskThreadNum使用原子变量，就不会产生多线程问题
			pThreadPool->m_runningTaskThreadNum++;
			pTask->doIt();
			pThreadPool->m_runningTaskThreadNum--;
		}
	}
}