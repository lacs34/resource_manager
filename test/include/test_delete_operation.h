/*
 * test_delete_operation.h
 *
 *  Created on: 2016Äê3ÔÂ16ÈÕ
 *      Author: Administrator
 */

#ifndef TEST_INCLUDE_TEST_DELETE_OPERATION_H_
#define TEST_INCLUDE_TEST_DELETE_OPERATION_H_

#include "memory_manager_interface.h"
#include <exception>

class TestDeleteOperation :
	public DeleteOperation {
public:
	long m_DeleteCount;

public:
	TestDeleteOperation() :
		m_DeleteCount(0L) {
	}
	virtual void DoDelete() override {
		++m_DeleteCount;
	}
};

class TestExternalRecordDeleteOperation :
	public DeleteOperation {
public:
	int *m_Record;

public:
	TestExternalRecordDeleteOperation(int *record) :
		m_Record(record) {
	}
	virtual void DoDelete() override {
		++(*m_Record);
	}
};

class TestDestructionDeleteOperation :
	public DeleteOperation {
private:
	int *m_DeleteCountRecorder;

public:
	TestDestructionDeleteOperation(int *recorder) :
		m_DeleteCountRecorder(recorder) {
	}
	virtual void DoDelete() override {
	}
	virtual ~TestDestructionDeleteOperation() {
		if (m_DeleteCountRecorder != nullptr) {
			++(*m_DeleteCountRecorder);
		}
	}
};

class TestDeleteOperationRaisedException :
	public std::exception {
};

class TestRaiseExceptionDeleteOperation :
	public DeleteOperation {
public:
	virtual void DoDelete() override {
		throw TestDeleteOperationRaisedException();
	}
};

#endif /* TEST_INCLUDE_TEST_DELETE_OPERATION_H_ */
