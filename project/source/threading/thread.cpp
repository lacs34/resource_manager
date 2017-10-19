/*
* thread.cpp
*
*  Created on: 2016Äê3ÔÂ25ÈÕ
*      Author: ÌÎ
*/

#include "threading/thread.h"
#include "predefined_smart_pointers.h"

static void ThreadStartProc(SmartPointer<Runnable> threadProc) {
	threadProc->Run();
}

Thread::Thread(SmartPointer<Runnable> threadProc) :
    m_ThreadProc(threadProc) {
}

void Thread::StartThread() {
	m_Thread.Initialize(ThreadStartProc, m_ThreadProc);
}

SmartPointer<Thread> Thread::Start(SmartPointer<Runnable> target) {
	AutoPointer<Thread, StdDeleteTraits> thread(new Thread(target));
	AutoPointer<StdDeleteOperation<Thread, StdDeleteTraits>, StdDeleteTraits> threadDeleteOperation(new StdDeleteOperation<Thread, StdDeleteTraits>(thread.Get()));
	AutoPointer<MultiThreadReferenceCountMemoryManager<StdDeleteTraits>, StdDeleteTraits> threadMemoryManager(new MultiThreadReferenceCountMemoryManager<StdDeleteTraits>(threadDeleteOperation.Get()));
	threadDeleteOperation.Release();
	SmartPointer<Thread> threadSmartPointer(thread.Release(), threadMemoryManager.Release());
	threadSmartPointer->StartThread();
	return threadSmartPointer;
}

bool Thread::IsTerminated() {
	// TODO: use native method to implement it
	return false;
}

void Thread::Join() {
	m_Thread->join();
}

void Thread::Join(TimeSpan maxTime) {
}

bool Thread::operator==(Thread &another) {
	return m_Thread->get_id() == another.m_Thread->get_id();
}