/*
 * test_destructor_invoked_object.h
 *
 *  Created on: 2016Äê3ÔÂ19ÈÕ
 *      Author: Administrator
 */

#ifndef TEST_INCLUDE_TEST_DESTRUCTOR_INVOKED_OBJECT_H_
#define TEST_INCLUDE_TEST_DESTRUCTOR_INVOKED_OBJECT_H_

class TestDestructorInvokedObject {
private:
	int *m_DestructCountRecorder;

public:
	TestDestructorInvokedObject(int *recorder) :
		m_DestructCountRecorder(recorder) {
	}
	~TestDestructorInvokedObject() {
		++(*m_DestructCountRecorder);
	}
};



#endif /* TEST_INCLUDE_TEST_DESTRUCTOR_INVOKED_OBJECT_H_ */
