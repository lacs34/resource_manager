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

#if !defined(DISABLE_C11)

template<typename OBJECT_TYPE, typename DESTROY_TRAITS>
using StdDeleteOperation = ObjectDeleteOperation<OBJECT_TYPE, StdDeleteTraits, DESTROY_TRAITS>;
template<typename OBJECT_TYPE, typename DESTROY_TRAITS>
using VectorDeleteOperation = ObjectDeleteOperation<OBJECT_TYPE, VectorDeleteTraits, DESTROY_TRAITS>;
template<typename OBJECT_TYPE, typename DESTROY_TRAITS>
using FreeDeleteOperation = ObjectDeleteOperation<OBJECT_TYPE, ObjectFreeTraits, DESTROY_TRAITS>;

#else

template<typename OBJECT_TYPE, typename DESTROY_TRAITS>
class StdDeleteOperation :
    public ObjectDeleteOperation<OBJECT_TYPE, StdDeleteTraits, DESTROY_TRAITS> {
public:
    StdDeleteOperation(OBJECT_TYPE *object) :
        ObjectDeleteOperation(object) {
    }
};

template<typename OBJECT_TYPE, typename DESTROY_TRAITS>
class VectorDeleteOperation :
    public ObjectDeleteOperation<OBJECT_TYPE, VectorDeleteTraits, DESTROY_TRAITS> {
public:
    VectorDeleteOperation(OBJECT_TYPE *object) :
        ObjectDeleteOperation(object) {
    }
};

template<typename OBJECT_TYPE, typename DESTROY_TRAITS>
class FreeDeleteOperation :
    public ObjectDeleteOperation<OBJECT_TYPE, ObjectFreeTraits, DESTROY_TRAITS> {
public:
    FreeDeleteOperation(OBJECT_TYPE *object) :
        ObjectDeleteOperation(object) {
    }
};

#endif

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

#if defined(DISABLE_C11)

class NewDeleteOperationTypeMismatchException :
	public std::exception {
};

template<typename ACTUAL_DELETE_OPERATION, typename DESTROY_TRAITS>
class ActivateDeleteOperation;
template<typename ACTUAL_DELETE_OPERATION, typename DESTROY_TRAITS>
void* operator new(size_t allocateBlockSize, ActivateDeleteOperation<ACTUAL_DELETE_OPERATION, DESTROY_TRAITS> &activateDeleteOperation);

#endif

template<typename CONTAINED_TYPE>
class LateInitialize {
private:
	union DummyUnion {
		DummyUnion() { }
		CONTAINED_TYPE m_Object;
		~DummyUnion() { }
	} m_ObjectUnion;

public:
	LateInitialize() {
	}

	template<typename ...CONSTRUCTOR_ARGS>
	void Initialize(CONSTRUCTOR_ARGS&&... args) {
		new(&m_ObjectUnion.m_Object) CONTAINED_TYPE(std::forward<CONSTRUCTOR_ARGS>(args)...);
	}

	CONTAINED_TYPE* operator ->() {
		return &m_ObjectUnion.m_Object;
	}

	void Destroy() {
		m_ObjectUnion.m_Object.~CONTAINED_TYPE();
	}

	~LateInitialize() { }
};

template<typename ACTUAL_DELETE_OPERATION, typename DESTROY_TRAITS>
class ActivateDeleteOperation :
	public DeleteOperation {
private:
	LateInitialize<ACTUAL_DELETE_OPERATION> m_ActualDeleteOperation;
	bool m_IsActivated;

#if defined(DISABLE_C11)

	void* Alloc() {
		if (m_IsActivated) {
			throw RepeatedActivationException();
		}
		m_IsActivated = true;
        return reinterpret_cast<void*>(&m_ActualDeleteOperation);
	}

#endif

public:
	ActivateDeleteOperation() :
		m_IsActivated(false) {
	}
#if !defined(DISABLE_C11)

	template<typename ...ARGS>
	void Activate(ARGS... args) {
		if (m_IsActivated) {
			throw RepeatedActivationException();
		}
		m_ActualDeleteOperation.Initialize(args...);
		m_IsActivated = true;
	}

#else

	friend void* operator new<ACTUAL_DELETE_OPERATION, DESTROY_TRAITS>(size_t allocateBlockSize, ActivateDeleteOperation<ACTUAL_DELETE_OPERATION, DESTROY_TRAITS> &activateDeleteOperation);

#endif
	virtual void DoDelete() override {
		AutoPointer<ActivateDeleteOperation<ACTUAL_DELETE_OPERATION, DESTROY_TRAITS>, DESTROY_TRAITS> deleteThis(this);
		if (m_IsActivated) {
			m_ActualDeleteOperation->DoDelete();
		}
	}
	virtual ~ActivateDeleteOperation() {
		if (m_IsActivated) {
			m_ActualDeleteOperation.Destroy();
		}
	}
};

#if defined(DISABLE_C11)
template<typename ACTUAL_DELETE_OPERATION, typename DESTROY_TRAITS>
void* operator new(size_t allocateBlockSize, ActivateDeleteOperation<ACTUAL_DELETE_OPERATION, DESTROY_TRAITS> &activateDeleteOperation) {
	if (sizeof(ACTUAL_DELETE_OPERATION) != allocateBlockSize) {
		throw NewDeleteOperationTypeMismatchException();
	}
	return activateDeleteOperation.Alloc();
}
#endif

#endif /* PROJECT_INCLUDE_DELETE_OPERATIONS_H_ */
