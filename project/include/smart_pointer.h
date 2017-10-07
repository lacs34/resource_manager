/*
 * smart_pointer.h
 *
 *  Created on: 2016��3��14��
 *      Author: Administrator
 */

#ifndef PROJECT_INCLUDE_SMART_POINTER_H_
#define PROJECT_INCLUDE_SMART_POINTER_H_

#include "memory_manager_interface.h"

template<typename POINTED_TYPE>
class SmartPointer {
private:
	POINTED_TYPE *m_Pointer;
	MemoryManager *m_Manager;

private:
	void Clear() {
		m_Pointer = nullptr;
		m_Manager = nullptr;
	}
	void Release() {
		if (m_Manager != nullptr) {
			m_Manager->DecreaseReference();
			Clear();
		}
	}
	inline bool EqualTo(SmartPointer<POINTED_TYPE> &pointer) {
		return m_Pointer == pointer.m_Pointer;
	}

public:
	SmartPointer(POINTED_TYPE *pointer, MemoryManager *manager) :
		m_Pointer(pointer), m_Manager(manager) {
	}
	SmartPointer(const SmartPointer<POINTED_TYPE> &pointer) :
		SmartPointer(pointer.m_Pointer, pointer.m_Manager) {
		if (m_Manager != nullptr) {
			m_Manager->IncreaseReference();
		}
	}
	SmartPointer(SmartPointer<POINTED_TYPE> &&pointer) :
		SmartPointer(pointer.m_Pointer, pointer.m_Manager) {
		pointer.Clear();
	}
	POINTED_TYPE* operator ->() {
		return m_Pointer;
	}
	const POINTED_TYPE* operator ->() const {
		return m_Pointer;
	}
	SmartPointer<POINTED_TYPE>& operator =(const SmartPointer<POINTED_TYPE> &pointer) {
		if (EqualTo(pointer)) {
			return *this;
		}
		MemoryManager *manager = pointer.m_Manager;
		manager->IncreaseReference();
		Clear();
		m_Pointer = pointer.m_Pointer;
		m_Manager = manager;
		return *this;
	}
	~SmartPointer() {
		Release();
	}
};

#endif /* PROJECT_INCLUDE_SMART_POINTER_H_ */