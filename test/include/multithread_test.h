/*
 * multithread_test.h
 *
 *  Created on: 2016��3��21��
 *      Author: Administrator
 */

#ifndef TEST_INCLUDE_MULTITHREAD_TEST_H_
#define TEST_INCLUDE_MULTITHREAD_TEST_H_

#include <time.h>
#if __struct_timespec_defined
#define _TIMESPEC_DEFINED
#endif
#include <thread>
#include <future>

class ThreadKernel {
public:
	virtual void Run() = 0;
	virtual ~ThreadKernel() { }
};
void f() // f sleeps for 1 second,
{ // then returns
}
template<unsigned THREAD_COUNT>
class MultiThreadController {
private:
	std::future<void> m_Futures[THREAD_COUNT];
	std::condition_variable m_StartCoindition;
	std::mutex m_StartMutex;
	std::atomic<unsigned> m_ReadyThreadCount;
	std::condition_variable m_ThreadReadyCoindition;

	static void ThreadStartFunction(ThreadKernel *kernel, MultiThreadController *controller) {
		std::unique_lock<std::mutex> lock(controller->m_StartMutex);
		++(controller->m_ReadyThreadCount);
		controller->m_ThreadReadyCoindition.notify_one();
		controller->m_StartCoindition.wait(lock);
		kernel->Run();
	}

	void StartThreads() {
		std::unique_lock<std::mutex> lock(m_StartMutex);
		while (m_ReadyThreadCount < THREAD_COUNT) {
			m_ThreadReadyCoindition.wait(lock);
		}
		m_StartCoindition.notify_all();
	}

public:
	MultiThreadController() {
	}
	void Run(ThreadKernel *kernels[THREAD_COUNT]) {
		m_ReadyThreadCount.store(0U);
		for (size_t i = 0; i < THREAD_COUNT; ++i) {
			m_Futures[i] = std::async(&ThreadStartFunction, kernels[i], this);
		}
		StartThreads();
	}
	void Run(ThreadKernel *kernel) {
		m_ReadyThreadCount.store(0U);
		for (unsigned i = 0; i < THREAD_COUNT; ++i) {
			m_Futures[i] = std::async(&ThreadStartFunction, kernel, this);
		}
		StartThreads();
	}
	void WaitToEnd() {
		for (size_t i = 0; i < THREAD_COUNT; ++i) {
			m_Futures[i].wait();
		}
	}
};
template<typename RUNNER>
class TestKernel :
	public ThreadKernel {
private:
	RUNNER m_Runner;

public:
	TestKernel(RUNNER runner) :
		m_Runner(runner) {

	}
	virtual void Run() {
		m_Runner();
	}
};
template<typename RUNNER>
TestKernel<RUNNER>* CreateTestKernel(RUNNER runner) {
	return new TestKernel<RUNNER>(runner);
}

#endif /* TEST_INCLUDE_MULTITHREAD_TEST_H_ */
