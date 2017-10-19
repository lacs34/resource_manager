/*
 * arrays.h
 *
 *  Created on: 2016Äê3ÔÂ25ÈÕ
 *      Author: ÌÎ
 */

#ifndef PROJECT_INCLUDE_ARRAYS_H_
#define PROJECT_INCLUDE_ARRAYS_H_

#include "memory_manager_interface.h"
#include "common_exceptions.h"
#include "collections/collections_interfaces.h"
#include "predefined_smart_pointers.h"
#include <cstddef>
#include <algorithm>

template<typename ELEMENT_TYPE>
class CArrayIterator :
	public Iterator<ELEMENT_TYPE> {
private:
	const ELEMENT_TYPE *m_StartAddress;
	size_t m_Length;
	MemoryManager *m_Manager;

public:
	CArrayIterator(const ELEMENT_TYPE *startAddress, std::size_t length, MemoryManager *manager = nullptr) :
		m_StartAddress(startAddress),
		m_Length(length),
		m_Manager(manager) {
		if (manager != nullptr) {
			manager->IncreaseReference();
		}
	}
	virtual bool MoveNext() override {
		if (m_Length == 0) {
			return false;
		}
		++m_StartAddress;
		--m_Length;
		return true;
	}
	virtual ELEMENT_TYPE GetCurrent() override {
		return *m_StartAddress;
	}
	virtual ~CArrayIterator() {
		if (m_Manager != nullptr) {
			m_Manager->DecreaseReference();
		}
	}
};

template<typename ELEMENT_TYPE>
class Array {
private:
	ELEMENT_TYPE *m_StartAddress;
	MemoryManager *m_Manager;
	size_t m_Length;

public:
	Array(ELEMENT_TYPE *start_address, MemoryManager *manager, size_t length) :
		m_StartAddress(start_address), m_Manager(manager), m_Length(length) {
	}
	Array(const Array<ELEMENT_TYPE> &source) :
		m_StartAddress(source.m_StartAddress), m_Manager(source.m_Manager), m_Length(source.m_Length) {
		if (m_Manager != nullptr) {
		    m_Manager->IncreaseReference();
		}
	}
	Array() :
		m_StartAddress(nullptr), m_Manager(&DummyManager), m_Length(0_size) {
	}
	inline ELEMENT_TYPE* GetAddress() const {
		return m_StartAddress;
	}
	inline size_t GetLength() const {
		return m_Length;
	}
	void CopyFrom(const ELEMENT_TYPE *array, size_t count) {
		size_t copyLength = std::min(m_Length, count);
		std::copy(array, array + copyLength, m_StartAddress);
	}
	inline void CopyFrom(const Array<ELEMENT_TYPE> array) {
		CopyFrom(array.m_StartAddress, array.m_Length);
	}
	void CopyTo(ELEMENT_TYPE *array, size_t count) {
		size_t copyLength = std::min(m_Length, count);
		std::copy(m_StartAddress, m_StartAddress + copyLength, array);
	}
	inline void CopyTo(Array<ELEMENT_TYPE> array) {
		CopyTo(array.m_StartAddress, array.m_Length);
	}
	inline ELEMENT_TYPE& GetElementAt(size_t index) {
		if (m_Length >= index) {
			throw OutOfRangeException();
		}
		return m_StartAddress[index];
	}
	virtual ~Array() {
		m_Manager->DecreaseReference();
	}

	virtual SmartPointer<Iterator<ELEMENT_TYPE>> GetIterator() {
		SmartPointer<Iterator<ELEMENT_TYPE>> iterator =
			StdPointer<CArrayIterator<ELEMENT_TYPE>>(m_StartAddress, m_Length, m_Manager)
			.Cast<Iterator<ELEMENT_TYPE>>();
		if (m_Manager != nullptr) {
            m_Manager->IncreaseReference();
		}
		return iterator;
	}
};

#endif /* PROJECT_INCLUDE_ARRAYS_H_ */
