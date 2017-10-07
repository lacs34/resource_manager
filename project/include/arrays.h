/*
 * arrays.h
 *
 *  Created on: 2016Äê3ÔÂ25ÈÕ
 *      Author: ÌÎ
 */

#ifndef PROJECT_INCLUDE_ARRAYS_H_
#define PROJECT_INCLUDE_ARRAYS_H_

#include "memory_manager_interface.h"
#include "exceptions.h"
#include <cstddef>
#include <algorithm>

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
		m_Manager->IncreaseReference();
	}
	inline ELEMENT_TYPE* GetAddress() {
		return m_StartAddress;
	}
	inline size_t GetLength() {
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
};

#endif /* PROJECT_INCLUDE_ARRAYS_H_ */
