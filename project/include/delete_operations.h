/*
 * delete_operations.h
 *
 *  Created on: 2016Äê3ÔÂ14ÈÕ
 *      Author: Administrator
 */

#ifndef PROJECT_INCLUDE_DELETE_OPERATIONS_H_
#define PROJECT_INCLUDE_DELETE_OPERATIONS_H_

#include "memory_manager_interface.h"
#include "delete_traits.h"
#include <exception>
#include <new>

class DummyDeleteOperation :
	public DeleteOperation {
public:
	virtual void DoDelete() override {
	}
};

template<typename OBJECT_TYPE, typename OBJECT_DESTROY_TRAITS, typename THIS_DESTROY_TRAITS>
class ObjectDeleteOperation :
	public DeleteOperation {
private:
	OBJECT_TYPE *m_Object;

public:
	ObjectDeleteOperation(OBJECT_TYPE *object) :
		m_Object(object) {
	}
	virtual void DoDelete() override {
		AutoPointer<ObjectDeleteOperation<OBJECT_TYPE, OBJECT_DESTROY_TRAITS, THIS_DESTROY_TRAITS>, THIS_DESTROY_TRAITS> deleteThis(this);
		OBJECT_DESTROY_TRAITS::DeleteObject(m_Object);
	}
};

template<typename OBJECT_TYPE, typename DESTROY_TRAITS>
using StdDeleteOperation = ObjectDeleteOperation<OBJECT_TYPE, StdDeleteTraits, DESTROY_TRAITS>;
template<typename OBJECT_TYPE, typename DESTROY_TRAITS>
using FreeDeleteOperation = ObjectDeleteOperation<OBJECT_TYPE, ObjectFreeTraits, DESTROY_TRAITS>;

template<typename DESTROY_TRAITS>
class ForwardDeleteOperation :
	public DeleteOperation {
private:
	DeleteOperation *m_Receiver;

public:
	ForwardDeleteOperation(DeleteOperation *receiver) :
		m_Receiver(receiver) {
	}
	virtual void DoDelete() override {
		AutoPointer<ForwardDeleteOperation<DESTROY_TRAITS>, DESTROY_TRAITS> deleteThis(this);
		m_Receiver->DoDelete();
	}
};

class NotActivatedException :
	public std::exception {
};

class RepeatedActivationException :
	public std::exception {
};

template<typename ACTUAL_DELETE_OPERATION, typename DESTROY_TRAITS>
class ActivateDeleteOperation :
	public DeleteOperation {
private:
	union {
		ACTUAL_DELETE_OPERATION m_ActualDeleteOperation;
	};
	bool m_IsActivated;

public:
	ActivateDeleteOperation() :
		m_IsActivated(false) {
	}
	template<typename ...ARGS>
	void Activate(ARGS... args) {
		if (m_IsActivated) {
			throw RepeatedActivationException();
		}
		new(&m_ActualDeleteOperation) ACTUAL_DELETE_OPERATION(args...);
		m_IsActivated = true;
	}
	virtual void DoDelete() override {
		AutoPointer<ActivateDeleteOperation<ACTUAL_DELETE_OPERATION, DESTROY_TRAITS>, DESTROY_TRAITS> deleteThis(this);
		if (m_IsActivated) {
			m_ActualDeleteOperation.DoDelete();
		}
	}
	virtual ~ActivateDeleteOperation() {
		if (m_IsActivated) {
			m_ActualDeleteOperation.~ACTUAL_DELETE_OPERATION();
		}
	}
};

#endif /* PROJECT_INCLUDE_DELETE_OPERATIONS_H_ */
