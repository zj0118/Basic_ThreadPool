#pragma once
#include<list>
#include<mutex>
#include<memory>
#include<vector>

class Task {
public:
	virtual void doIt();
};

class ThreadPool {
public:
	ThreadPool();
	~ThreadPool();

	void init(int threadNum = 6);
	void destory();

	void addTask(Task* task);

private:
	static void threadFunc(void* threadParam);

private:
	bool											m_exitFlag;
	//任务队列
	std::list<Task*>								m_taskQueue;
	std::mutex										m_taskMutex;
	std::vector<std::shared_ptr<std::thread>>		m_thread;

};