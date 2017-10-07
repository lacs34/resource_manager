/*
 * memory_managers.h
 *
 *  Created on: 2016Äê3ÔÂ13ÈÕ
 *      Author: Administrator
 */

#ifndef PROJECT_INCLUDE_MEMORY_MANAGERS_H_
#define PROJECT_INCLUDE_MEMORY_MANAGERS_H_

#include <atomic>
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
class MultiThreadTraits {
public:
	typedef std::atomic<unsigned long> REFERENCE_COUNT_TYPE;
	static void Increase(REFERENCE_COUNT_TYPE &referenceCount) {
		++referenceCount;
	}
	static unsigned long Decrease(REFERENCE_COUNT_TYPE &referenceCount) {
		
		unsigned long updatedValue = referenceCount.fetch_sub(1L, std::memory_order_seq_cst);
		return updatedValue;
	}
};

template<typename DESTROY_TRAITS>
using SingleThreadReferenceCountMemoryManager = TReferenceCountMemoryManager<SingleThreadTraits, DESTROY_TRAITS>;

template<typename DESTROY_TRAITS>
using MultiThreadReferenceCountMemoryManager = TReferenceCountMemoryManager<MultiThreadTraits, DESTROY_TRAITS>;

#endif /* PROJECT_INCLUDE_MEMORY_MANAGERS_H_ */
