/*
 * memory_managers.h
 *
 *  Created on: 2016Äê3ÔÂ13ÈÕ
 *      Author: Administrator
 */

#ifndef PROJECT_INCLUDE_MEMORY_MANAGERS_H_
#define PROJECT_INCLUDE_MEMORY_MANAGERS_H_

#include "memory_manager_interface.h"
#include "delete_traits.h"

class DummyMemoryManager :
	public MemoryManager {
public:
	virtual void IncreaseReference() override;
	virtual void DecreaseReference() override;
};
extern DummyMemoryManager DummyManager;

template<typename THREAD_TRAITS, typename DESTROY_TRAITS>
class TReferenceCountMemoryManager :
	public MemoryManager {
private:
	DeleteOperation *m_DeleteOperation;
	typename THREAD_TRAITS::REFERENCE_COUNT_TYPE m_ReferenceCount;

public:
	TReferenceCountMemoryManager(DeleteOperation *deleteOperation) :
		m_DeleteOperation(deleteOperation),
		m_ReferenceCount(1L) {
	}
	virtual void IncreaseReference() override {
		THREAD_TRAITS::Increase(m_ReferenceCount);
	}
	virtual void DecreaseReference() override {
		unsigned long updatedReferenceCount = THREAD_TRAITS::Decrease(m_ReferenceCount);
		if (updatedReferenceCount == 0L) {
			m_DeleteOperation->DoDelete();
			DESTROY_TRAITS::DeleteObject(this);
		}
	}
};
#include <iostream>
class SingleThreadTraits {
public:
	typedef unsigned long REFERENCE_COUNT_TYPE;
	static void Increase(REFERENCE_COUNT_TYPE &referenceCount) {
		++referenceCount;
	}
	static unsigned long Decrease(REFERENCE_COUNT_TYPE &referenceCount) {
		--referenceCount;
		return referenceCount;
	}
};

#if !defined(DISABLE_C11)

#include <atomic>

class MultiThreadTraits {
public:
	typedef std::atomic<unsigned long> REFERENCE_COUNT_TYPE;
	static void Increase(REFERENCE_COUNT_TYPE &referenceCount) {
		++referenceCount;
	}
	static unsigned long Decrease(REFERENCE_COUNT_TYPE &referenceCount) {
		unsigned long updatedValue = referenceCount.fetch_sub(1L, std::memory_order_seq_cst);
		return updatedValue - 1;
	}
};

template<typename DESTROY_TRAITS>
using SingleThreadReferenceCountMemoryManager = TReferenceCountMemoryManager<SingleThreadTraits, DESTROY_TRAITS>;

template<typename DESTROY_TRAITS>
using MultiThreadReferenceCountMemoryManager = TReferenceCountMemoryManager<MultiThreadTraits, DESTROY_TRAITS>;

#else

#if defined(WINDOWS_PLATFORM)

#include <Windows.h>
class MultiThreadTraits {
public:
	typedef LONG REFERENCE_COUNT_TYPE;
	static void Increase(REFERENCE_COUNT_TYPE &referenceCount) {
		InterlockedIncrement(&referenceCount);
	}
	static unsigned long Decrease(REFERENCE_COUNT_TYPE &referenceCount) {
		LONG updatedValue = InterlockedDecrement(&referenceCount);
		return updatedValue;
	}
};

#endif

template<typename DESTROY_TRAITS>
class SingleThreadReferenceCountMemoryManager :
	public TReferenceCountMemoryManager<SingleThreadTraits, DESTROY_TRAITS> {
public:
	SingleThreadReferenceCountMemoryManager(DeleteOperation *deleteOperation) :
	    TReferenceCountMemoryManager(deleteOperation) {
	}
};

template<typename DESTROY_TRAITS>
class MultiThreadReferenceCountMemoryManager :
	public TReferenceCountMemoryManager<MultiThreadTraits, DESTROY_TRAITS> {
public:
	MultiThreadReferenceCountMemoryManager(DeleteOperation *deleteOperation) :
	    TReferenceCountMemoryManager(deleteOperation) {
	}
};

#endif



#endif /* PROJECT_INCLUDE_MEMORY_MANAGERS_H_ */
