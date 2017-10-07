/*
 * multithread_test.h
 *
 *  Created on: 2016Äê3ÔÂ21ÈÕ
 *      Author: Administrator
 */

#ifndef TEST_INCLUDE_MULTITHREAD_TEST_H_
#define TEST_INCLUDE_MULTITHREAD_TEST_H_

#include <time.h>
#if __struct_timespec_defined
#define _TIMESPEC_DEFINED
#endif
#include <pthread.h>

class ThreadKernel {
public:
	virtual void Run() = 0;
	virtual ~ThreadKernel() { }
};

template<unsigned THREAD_COUNT>
class MultiThreadController {
private:
	pthread_t m_Threads[THREAD_COUNT];
	pthread_barrier_t m_StartBarrier;
	struct ThreadArgument {
		ThreadKernel *m_Kernel;
		MultiThreadController *m_Controller;
	};
	ThreadArgument m_Arguments[THREAD_COUNT];

	static void* ThreadStartFunction(void *arg) {
		ThreadArgument *threadArgument = reinterpret_cast<ThreadArgument*>(arg);
		pthread_barrier_wait(&threadArgument->m_Controller->m_StartBarrier);
		threadArgument->m_Kernel->Run();
		return nullptr;
	}

public:
	MultiThreadController() {
	}
	void Run(ThreadKernel *kernels[THREAD_COUNT]) {
		int err = pthread_barrier_init(&m_StartBarrier, nullptr, THREAD_COUNT);
		if (err != 0) {
			throw PosixException(err);
		}
		for (size_t i = 0; i < THREAD_COUNT; ++i) {
			m_Arguments[i].m_Kernel = kernels[i];
			m_Arguments[i].m_Controller = this;
			err = pthread_create(m_Threads + i, nullptr, &ThreadStartFunction, reinterpret_cast<void*>(m_Arguments + i));
			if (err != 0) {
				throw PosixException(err);
			}
		}
	}
	void Run(ThreadKernel *kernels) {
		int err = pthread_barrier_init(&m_StartBarrier, nullptr, THREAD_COUNT);
		if (err != 0) {
			throw PosixException(err);
		}
		for (unsigned i = 0; i < THREAD_COUNT; ++i) {
			m_Arguments[i].m_Kernel = kernels;
			m_Arguments[i].m_Controller = this;
			err = pthread_create(m_Threads + i, nullptr, &ThreadStartFunction, reinterpret_cast<void*>(m_Arguments + i));
			if (err != 0) {
				throw PosixException(err);
			}
		}
	}
	void WaitToEnd() {
		for (size_t i = 0; i < THREAD_COUNT; ++i) {
			int err = pthread_join(m_Threads[i], nullptr);
			if (err != 0) {
				throw PosixException(err);
			}
		}
		int err = pthread_barrier_destroy(&m_StartBarrier);
		if (err != 0) {
			throw PosixException(err);
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
