/*
 * predefined_array.h
 *
 *  Created on: 2016Äê3ÔÂ25ÈÕ
 *      Author: ÌÎ
 */

#ifndef PROJECT_INCLUDE_PREDEFINED_ARRAY_H_
#define PROJECT_INCLUDE_PREDEFINED_ARRAY_H_

#include "arrays.h"
#include "delete_traits.h"
#include "delete_operations.h"
#include "memory_managers.h"

template<typename ELEMENT_TYPE>
class StdArray :
	public Buffer<ELEMENT_TYPE> {
public:
	static Buffer<ELEMENT_TYPE> Create(size_t length) {
		AutoPointer<ELEMENT_TYPE, VectorDeleteTraits> pointer(new ELEMENT_TYPE[length]);
		AutoPointer<VectorDeleteOperation<ELEMENT_TYPE, VectorDeleteTraits>, VectorDeleteTraits> deleteOperation(
			new VectorDeleteOperation<ELEMENT_TYPE, VectorDeleteTraits>(pointer.Get()));
		AutoPointer<MultiThreadReferenceCountMemoryManager<VectorDeleteTraits>, VectorDeleteTraits> manager(
			new MultiThreadReferenceCountMemoryManager<VectorDeleteTraits>(deleteOperation.Get()));
		deleteOperation.Release();
		Buffer<ELEMENT_TYPE> createdBuffer(pointer.Release(), std::move(*manager.Release()), length);
		return createdBuffer;
	}
	StdArray(size_t length) :
		Buffer<ELEMENT_TYPE>(Create(length)) {
	}
};

#endif /* PROJECT_INCLUDE_PREDEFINED_ARRAY_H_ */
