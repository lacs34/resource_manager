/*
 * arrays.h
 *
 *  Created on: 2016Äê3ÔÂ25ÈÕ
 *      Author: ÌÎ
 */

#ifndef PROJECT_INCLUDE_ARRAYS_H_
#define PROJECT_INCLUDE_ARRAYS_H_

#include "memory_manager_interface.h"

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
};

#endif /* PROJECT_INCLUDE_ARRAYS_H_ */
