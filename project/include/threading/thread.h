/*
* thread.h
*
*  Created on: 2016Äê3ÔÂ25ÈÕ
*      Author: ÌÎ
*/

#ifndef THREAD_H_
#define THREAD_H_

#include "smart_pointer.h"
#include "date_time.h"
#include "delete_operations.h"
#include <thread>

class Runnable {
public:
	virtual void Run() = 0;
	virtual ~Runnable() { };
};

class Thread {
private:
	LateInitialize<std::thread> m_Thread;
	SmartPointer<Runnable> m_ThreadProc;
	Thread(SmartPointer<Runnable> threadProc);
	void StartThread();

public:
	static SmartPointer<Thread> Start(SmartPointer<Runnable> target);
	void Join();
	bool operator==(Thread &another);
	bool IsTerminated();
	void Join(TimeSpan maxTime);
};

#endif // !THREAD_H_



