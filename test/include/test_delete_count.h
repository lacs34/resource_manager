/*
 * test_delete_count.h
 *
 *  Created on: 2016Äê3ÔÂ19ÈÕ
 *      Author: Administrator
 */

#ifndef TEST_INCLUDE_TEST_DELETE_COUNT_H_
#define TEST_INCLUDE_TEST_DELETE_COUNT_H_

class TestDeleteCountObject {
private:
	int *m_DeleteCountRecorder;

public:
	TestDeleteCountObject(int *recorder) :
		m_DeleteCountRecorder(recorder) {
	}
	static void operator delete(void *address) {
		TestDeleteCountObject *object = reinterpret_cast<TestDeleteCountObject*>(address);
		if (object != nullptr) {
			++(*object->m_DeleteCountRecorder);
			::operator delete(address);
		}
	}
};



#endif /* TEST_INCLUDE_TEST_DELETE_COUNT_H_ */
