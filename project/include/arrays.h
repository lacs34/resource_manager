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
class Buffer {
private:
	ELEMENT_TYPE *m_StartAddress;
	ManagerHolder m_Manager;
	size_t m_Length;

public:
	Buffer(ELEMENT_TYPE *start_address, ResourceManager *manager, size_t length) :
		m_StartAddress(start_address), m_Manager(manager), m_Length(length) {
	}
	Buffer(const Buffer<ELEMENT_TYPE> &source) :
		m_StartAddress(source.m_StartAddress), m_Manager(source.m_Manager), m_Length(source.m_Length) {
	}
	Buffer() :
		m_StartAddress(nullptr), m_Manager(&DummyManager), m_Length(0_size) {
	}
	inline ELEMENT_TYPE* GetAddress() const {
		return m_StartAddress;
	}
	inline size_t GetLength() const {
		return m_Length;
	}
	inline ResourceManager* GetManager() const {
		return m_Manager;
	}
	void CopyFrom(const ELEMENT_TYPE *array, size_t count) {
		size_t copyLength = std::min(m_Length, count);
		std::copy(array, array + copyLength, m_StartAddress);
	}
	inline void CopyFrom(const Buffer<ELEMENT_TYPE> array) {
		CopyFrom(array.m_StartAddress, array.m_Length);
	}
	void CopyTo(ELEMENT_TYPE *array, size_t count) {
		size_t copyLength = std::min(m_Length, count);
		std::copy(m_StartAddress, m_StartAddress + copyLength, array);
	}
	inline void CopyTo(Buffer<ELEMENT_TYPE> array) {
		CopyTo(array.m_StartAddress, array.m_Length);
	}
	inline ELEMENT_TYPE& GetElementAt(size_t index) {
		if (m_Length >= index) {
			throw OutOfRangeException();
		}
		return m_StartAddress[index];
	}
	inline Buffer<ELEMENT_TYPE> GetSubBuffer(std::size_t fromIndex, std::size_t length) {
		if (from + length > m_Length) {
			throw OutOfRangeException();
		}
		if (length == 0_size) {
			return Buffer<ELEMENT_TYPE>();
		}
		return Buffer<ELEMENT_TYPE>(m_StartAddress + fromIndex, m_Manager, length);
	}

	virtual SmartPointer<Iterator<ELEMENT_TYPE>> GetIterator();
};

template<typename ELEMENT_TYPE, std::size_t ELEMENT_COUNT>
class ScopedArray {
private:
	ELEMENT_TYPE m_Element[ELEMENT_COUNT];

public:
	inline ELEMENT_TYPE* GetAddress() const {
		return m_Element;
	}
	inline size_t GetLength() const {
		return ELEMENT_COUNT;
	}
	inline ResourceManager* GetManager() const {
		return &DummyManager;
	}
	void CopyFrom(const ELEMENT_TYPE *array, size_t count) {
		size_t copyLength = std::min(ELEMENT_COUNT, count);
		std::copy(array, array + copyLength, m_Element);
	}
	inline void CopyFrom(const Buffer<ELEMENT_TYPE> array) {
		CopyFrom(array.m_StartAddress, array.m_Length);
	}
	void CopyTo(ELEMENT_TYPE *array, size_t count) {
		size_t copyLength = std::min(ELEMENT_COUNT, count);
		std::copy(m_Element, m_Element + copyLength, array);
	}
	inline void CopyTo(Buffer<ELEMENT_TYPE> array) {
		CopyTo(array.m_StartAddress, array.m_Length);
	}
	inline ELEMENT_TYPE& GetElementAt(size_t index) {
		if (ELEMENT_COUNT >= index) {
			throw OutOfRangeException();
		}
		return m_Element[index];
	}
	inline Buffer<ELEMENT_TYPE> GetSubBuffer(std::size_t fromIndex, std::size_t length) {
		if (from + length > ELEMENT_COUNT) {
			throw OutOfRangeException();
		}
		if (length == 0_size) {
			return Buffer<ELEMENT_TYPE>();
		}
		return Buffer<ELEMENT_TYPE>(m_Element + fromIndex, &DummyManager, length);
	}
	inline operator Buffer<ELEMENT_TYPE>() {
		return Buffer<ELEMENT_TYPE>(m_Element, &DummyManager, ELEMENT_COUNT);
	}

	virtual SmartPointer<Iterator<ELEMENT_TYPE>> GetIterator();
};

template<typename ELEMENT_TYPE>
class CBufferIterator :
	public Iterator<ELEMENT_TYPE> {
private:
	const ELEMENT_TYPE *m_StartAddress;
	size_t m_Length;
	ManagerHolder m_Manager;

public:
	CBufferIterator(const ELEMENT_TYPE *startAddress, std::size_t length, ResourceManager *manager = nullptr) :
		m_StartAddress(startAddress - 1),
		m_Length(length),
		m_Manager(manager) {
	}
	CBufferIterator(const Buffer<ELEMENT_TYPE> buffer) :
		CArrayIterator(buffer.GetAddress(), buffer.GetLength(), buffer.GetManager()) {
	}
	template<std::size_t BUFFER_SIZE>
	CBufferIterator(const ELEMENT_TYPE (&buffer)[BUFFER_SIZE], ResourceManager *manager = nullptr) :
		CArrayIterator(buffer, BUFFER_SIZE, manager) {
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
};

template<typename ELEMENT_TYPE>
SmartPointer<Iterator<ELEMENT_TYPE>> Buffer<ELEMENT_TYPE>::GetIterator() {
	SmartPointer<Iterator<ELEMENT_TYPE>> iterator =
		StdPointer<CBufferIterator<ELEMENT_TYPE>>(m_StartAddress, m_Length, m_Manager)
		.Cast<Iterator<ELEMENT_TYPE>>();
	return iterator;
}

template<typename ELEMENT_TYPE, std::size_t ELEMENT_COUNT>
SmartPointer<Iterator<ELEMENT_TYPE>> ScopedArray<ELEMENT_TYPE, ELEMENT_COUNT>::GetIterator() {
	SmartPointer<Iterator<ELEMENT_TYPE>> iterator =
		StdPointer<CBufferIterator<ELEMENT_TYPE>>(m_Element)
		.Cast<Iterator<ELEMENT_TYPE>>();
	return iterator;
}

#endif /* PROJECT_INCLUDE_ARRAYS_H_ */
