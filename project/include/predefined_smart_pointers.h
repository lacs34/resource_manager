/*
 * predefined_smart_pointers.h
 *
 *  Created on: 2016Äê3ÔÂ21ÈÕ
 *      Author: Administrator
 */

#ifndef PROJECT_INCLUDE_PREDEFINED_SMART_POINTERS_H_
#define PROJECT_INCLUDE_PREDEFINED_SMART_POINTERS_H_

#include "smart_pointer.h"
#include "delete_traits.h"
#include "delete_operations.h"
#include "memory_managers.h"

class SelfManager {
public:
	SelfManager() {
	}
};

template<typename OBJECT_TYPE, typename ...LEFTOVER_ARGS>
class ObjectCreator;

template<typename OBJECT_TYPE, typename ...LEFTOVER_ARGS>
class ObjectCreator<OBJECT_TYPE, SelfManager&, LEFTOVER_ARGS...> {
public:
	template<typename ...CONVERTED_ARGS>
	static inline OBJECT_TYPE* CreateObject(MemoryManager *manager, CONVERTED_ARGS... converted, SelfManager &current, LEFTOVER_ARGS... leftover) {
		ObjectCreator<OBJECT_TYPE, LEFTOVER_ARGS...>::CreateObject(manager, converted..., manager, leftover...);
	}
};

template<typename OBJECT_TYPE, typename CURRENT_ARG, typename ...LEFTOVER_ARGS>
class ObjectCreator<OBJECT_TYPE, CURRENT_ARG, LEFTOVER_ARGS...> {
public:
	template<typename ...CONVERTED_ARGS>
	static inline OBJECT_TYPE* CreateObject(MemoryManager *manager, CONVERTED_ARGS... converted, CURRENT_ARG current, LEFTOVER_ARGS... leftover) {
		ObjectCreator<OBJECT_TYPE, LEFTOVER_ARGS...>::CreateObject(manager, converted..., current, leftover...);
	}
};

template<typename OBJECT_TYPE>
class ObjectCreator<OBJECT_TYPE> {
public:
	template<typename ...CONVERTED_ARGS>
	static inline OBJECT_TYPE* CreateObject(MemoryManager *manager, CONVERTED_ARGS... converted) {
		return new OBJECT_TYPE(converted...);
	}
};

template<typename POINTED_TYPE>
class StdPointer :
	public SmartPointer<POINTED_TYPE> {
public:
	template<typename ...ARGS>
	static SmartPointer<POINTED_TYPE> Create(ARGS... args) {
		AutoPointer<ActivateDeleteOperation<StdDeleteOperation<POINTED_TYPE, DummyTraits>, StdDeleteTraits>, StdDeleteTraits> deleteOperation(
			new ActivateDeleteOperation<StdDeleteOperation<POINTED_TYPE, DummyTraits>, StdDeleteTraits>);
		AutoPointer<MultiThreadReferenceCountMemoryManager<StdDeleteTraits>, StdDeleteTraits> manager(
			new MultiThreadReferenceCountMemoryManager<StdDeleteTraits>(deleteOperation.Get()));
		AutoPointer<POINTED_TYPE, StdDeleteTraits> pointer(ObjectCreator<POINTED_TYPE, ARGS...>::CreateObject(static_cast<MemoryManager*>(manager.Get()), args...));
		deleteOperation->Activate(pointer.Get());
		deleteOperation.Release();
		return SmartPointer<POINTED_TYPE>(pointer.Release(), manager.Release());
	}
	template<typename ...ARGS>
	StdPointer(ARGS... args) :
		SmartPointer<POINTED_TYPE>(Create<ARGS...>(args...)) {
	}
};

#endif /* PROJECT_INCLUDE_PREDEFINED_SMART_POINTERS_H_ */
