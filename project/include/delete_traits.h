/*
 * delete_traits.h
 *
 *  Created on: 2016Äê3ÔÂ14ÈÕ
 *      Author: Administrator
 */

#ifndef PROJECT_INCLUDE_DELETE_TRAITS_H_
#define PROJECT_INCLUDE_DELETE_TRAITS_H_

#include <cstdlib>

template<typename OBJECT_TYPE, typename DESTROY_TRAITS>
class AutoPointer {
private:
	OBJECT_TYPE *m_Pointer;

public:
	AutoPointer(OBJECT_TYPE *pointer) :
		m_Pointer(pointer) {
	}

	inline OBJECT_TYPE* Get() {
		return m_Pointer;
	}

	inline OBJECT_TYPE* Release() {
		OBJECT_TYPE *pointer = m_Pointer;
		m_Pointer = nullptr;
		return pointer;
	}

	inline OBJECT_TYPE* operator ->() {
		return m_Pointer;
	}

	~AutoPointer() {
		if (m_Pointer != nullptr) {
			DESTROY_TRAITS::DeleteObject(m_Pointer);
		}
	}
};

class DummyTraits {
public:
	template<typename OBJECT_TYPE>
	static void DeleteObject(OBJECT_TYPE *object) {
	}
};

class StdDeleteTraits {
public:
	template<typename OBJECT_TYPE>
	static void DeleteObject(OBJECT_TYPE *object) {
		delete object;
	}
};

class VectorDeleteTraits {
public:
	template<typename OBJECT_TYPE>
	static void DeleteObject(OBJECT_TYPE *object) {
		delete[] object;
	}
};

class DestructTraits {
public:
	template<typename OBJECT_TYPE>
	static void DeleteObject(OBJECT_TYPE *object) {
		object->~OBJECT_TYPE();
	}
};

class RawFreeTraits {
public:
	template<typename OBJECT_TYPE>
	static void DeleteObject(OBJECT_TYPE *object) {
		free(reinterpret_cast<void*>(object));
	}
};

class ObjectFreeTraits {
public:
	template<typename OBJECT_TYPE>
	static void DeleteObject(OBJECT_TYPE *object) {
		AutoPointer<OBJECT_TYPE, RawFreeTraits> freeObject(object);
		object->~OBJECT_TYPE();
	}
};

#endif /* PROJECT_INCLUDE_DELETE_TRAITS_H_ */
