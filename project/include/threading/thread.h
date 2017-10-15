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

class Runnable {
public:
	virtual void Run() = 0;
	virtual ~Runnable() { };
};

class Thread {
private:
	Thread();
public:
	static SmartPointer<Thread> Start(SmartPointer<Runnable> target);
	bool IsTerminated();
	void Join();
	void Join(TimeSpan maxTime);
	bool operator==(Thread &another);
};

#endif // !THREAD_H_



