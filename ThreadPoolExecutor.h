#pragma once

#include<condition_variable>
#include<list>
#include<mutex>
#include<memory>
#include<vector>

class Task {
public:
	virtual void doIt();
};

class ThreadPoolExcutor {
public:
	ThreadPoolExcutor();
	~ThreadPoolExcutor();

	void init(int coreThreadNum = 6,int maxThreadNum,int maxTaskNum);
	void destory();

	bool addTask(Task* task);

private:
	static void threadFunc(void* threadParam);

private:
	bool											m_exitFlag;
	//任务队列
	std::list<Task*>								m_taskQueue;
	std::mutex										m_taskMutex;
	std::condition_variable							m_taskCV;
	std::vector<std::shared_ptr<std::thread>>		m_coreThreads;
	std::vector<std::shared_ptr<std::thread>>		m_extraThreads;
	int												m_coreThreadNum;
	int												m_maxThreadNum;
	int												m_maxTaskNum;
	//当前正在运行任务的线程数目
	std::atomic<int>								m_runningTaskThreadNum;
};