/*
 * test_delete_traits.h
 *
 *  Created on: 2016Äê3ÔÂ16ÈÕ
 *      Author: Administrator
 */

#ifndef TEST_INCLUDE_TEST_DELETE_TRAITS_H_
#define TEST_INCLUDE_TEST_DELETE_TRAITS_H_

#include <cstddef>
#include <exception>
#include "gtest.h"

template<typename OBJECT_TYPE>
class TestDeleteTraitsObjectContainer {
public:
	OBJECT_TYPE m_ObjectToBeDeleted;
	long m_DeleteCount;
	OBJECT_TYPE *m_ObjectPointer;
	template<typename ...ARGS>
	TestDeleteTraitsObjectContainer(ARGS... args) :
		m_ObjectToBeDeleted(args...),
		m_DeleteCount(0L),
		m_ObjectPointer(&m_ObjectToBeDeleted) {
	}
	OBJECT_TYPE* operator ->() {
		return &m_ObjectToBeDeleted;
	}
};

class TestDeleteTraits {
private:
	template<typename OBJECT_TYPE>
	static TestDeleteTraitsObjectContainer<OBJECT_TYPE>* ConvertInternalObjectAdress(OBJECT_TYPE *object) {
		size_t objectOffset =  offsetof(TestDeleteTraitsObjectContainer<OBJECT_TYPE>, m_ObjectToBeDeleted);
		return (TestDeleteTraitsObjectContainer<OBJECT_TYPE>*)(((long)object) - objectOffset);
	}

public:
	template<typename OBJECT_TYPE>
	static void DeleteObject(OBJECT_TYPE *object) {
		TestDeleteTraitsObjectContainer<OBJECT_TYPE> *traits = ConvertInternalObjectAdress(object);
		ASSERT_EQ(object, traits->m_ObjectPointer);
		++traits->m_DeleteCount;
	}
};

class TestDeleteTraitsRaisedException :
	public std::exception {
};

class TestRaiseExceptionDeleteTraits {
public:
	template<typename OBJECT_TYPE>
	static void DeleteObject(OBJECT_TYPE *object) {
		throw TestDeleteTraitsRaisedException();
	}
};

#endif /* TEST_INCLUDE_TEST_DELETE_TRAITS_H_ */
