/*
* thread.cpp
*
*  Created on: 2016Äê3ÔÂ25ÈÕ
*      Author: ÌÎ
*/

#include "threading/thread.h"
#include "predefined_smart_pointers.h"
#include "exceptions.h"

static void ThreadStartProc(SmartPointer<Runnable> threadProc) {
	threadProc->Run();
}

Thread::Thread(SmartPointer<Runnable> threadProc) :
    m_ThreadProc(std::move(threadProc)) {
}

void Thread::StartThread() {
	m_Thread.Initialize(ThreadStartProc, m_ThreadProc);
}

SmartPointer<Thread> Thread::Start(SmartPointer<Runnable> target) {
	AutoPointer<Thread, StdDeleteTraits> thread(new Thread(target));
	AutoPointer<StdDeleteOperation<Thread, StdDeleteTraits>, StdDeleteTraits> threadDeleteOperation(new StdDeleteOperation<Thread, StdDeleteTraits>(thread.Get()));
	AutoPointer<MultiThreadReferenceCountMemoryManager<StdDeleteTraits>, StdDeleteTraits> threadMemoryManager(new MultiThreadReferenceCountMemoryManager<StdDeleteTraits>(threadDeleteOperation.Get()));
	threadDeleteOperation.Release();
	SmartPointer<Thread> threadSmartPointer(std::move(*threadMemoryManager.Release()), thread.Release());
	threadSmartPointer->StartThread();
	return threadSmartPointer;
}

void Thread::Join() {
	m_Thread->join();
}

bool Thread::operator==(Thread &another) {
	return m_Thread->get_id() == another.m_Thread->get_id();
}

#if defined(WINDOWS_PLATFORM)

#include "windows_platform.h"

bool Thread::IsTerminated() {
	HANDLE threadHandle = m_Thread->native_handle();
	DWORD exitCode;
	BOOL succeed = GetExitCodeThread(threadHandle, &exitCode);
	if (!succeed) {
		ThrowLastErrorException();
	}
	return exitCode != STILL_ACTIVE;
}

void Thread::Join(TimeSpan maxTime) {
	HANDLE threadHandle = m_Thread->native_handle();
	Int64 milliseconds = maxTime.GetTotalMillisecond();
	DWORD waitResult = WaitForSingleObject(threadHandle, (DWORD)milliseconds);
	if (waitResult == WAIT_FAILED) {
		ThrowLastErrorException();
	}
}

#endif // WINDOWS_PLATFORM