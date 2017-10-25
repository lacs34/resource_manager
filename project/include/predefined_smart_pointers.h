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
#include "self_manager.h"

#if !defined(DISABLE_C11)

template<typename OBJECT_TYPE, typename ...LEFTOVER_ARGS>
class ObjectCreator;

template<typename OBJECT_TYPE, typename ...LEFTOVER_ARGS>
class ObjectCreator<OBJECT_TYPE, const SelfManager*&, LEFTOVER_ARGS...> {
public:
	template<typename ...CONVERTED_ARGS>
	static inline OBJECT_TYPE* CreateObject(ResourceManager *manager, const SelfManager *&current, LEFTOVER_ARGS&&... leftover, CONVERTED_ARGS&&... converted) {
		return ObjectCreator<OBJECT_TYPE, LEFTOVER_ARGS...>::CreateObject(manager, std::forward<LEFTOVER_ARGS>(leftover)..., std::forward<CONVERTED_ARGS>(converted)..., manager);
	}
};

template<typename OBJECT_TYPE>
class ObjectCreator<OBJECT_TYPE, const SelfManager*&> {
public:
	template<typename ...CONVERTED_ARGS>
	static inline OBJECT_TYPE* CreateObject(ResourceManager *manager, const SelfManager *&current, CONVERTED_ARGS&&... converted) {
		return new OBJECT_TYPE(std::forward<CONVERTED_ARGS>(converted)..., manager);
	}
};

template<typename OBJECT_TYPE, typename CURRENT_ARG, typename ...LEFTOVER_ARGS>
class ObjectCreator<OBJECT_TYPE, CURRENT_ARG, LEFTOVER_ARGS...> {
public:
	template<typename ...CONVERTED_ARGS>
	static inline OBJECT_TYPE* CreateObject(ResourceManager *manager, CURRENT_ARG &&current, LEFTOVER_ARGS&&... leftover, CONVERTED_ARGS&&... converted) {
		return ObjectCreator<OBJECT_TYPE, LEFTOVER_ARGS...>::CreateObject(manager, std::forward<LEFTOVER_ARGS>(leftover)..., std::forward<CONVERTED_ARGS>(converted)..., std::forward<CURRENT_ARG>(current));
	}
};

template<typename OBJECT_TYPE>
class ObjectCreator<OBJECT_TYPE> {
public:
	template<typename ...CONVERTED_ARGS>
	static inline OBJECT_TYPE* CreateObject(ResourceManager *manager, CONVERTED_ARGS&&... converted) {
		return new OBJECT_TYPE(std::forward<CONVERTED_ARGS>(converted)...);
	}
};

#endif

template<typename POINTED_TYPE>
class StdPointer :
	public SmartPointer<POINTED_TYPE> {
public:
	template<typename ...ARGS>
	static SmartPointer<POINTED_TYPE> Create(ARGS&&... args) {
		AutoPointer<ActivateDeleteOperation<StdDeleteOperation<POINTED_TYPE, DummyTraits>, StdDeleteTraits>, StdDeleteTraits> deleteOperation(
			new ActivateDeleteOperation<StdDeleteOperation<POINTED_TYPE, DummyTraits>, StdDeleteTraits>);
		AutoPointer<MultiThreadReferenceCountMemoryManager<StdDeleteTraits>, StdDeleteTraits> manager(
			new MultiThreadReferenceCountMemoryManager<StdDeleteTraits>(deleteOperation.Get()));
		AutoPointer<POINTED_TYPE, StdDeleteTraits> pointer(ObjectCreator<POINTED_TYPE, ARGS&&...>::CreateObject(static_cast<ResourceManager*>(manager.Get()), std::forward<ARGS>(args)...));
		deleteOperation->Activate(pointer.Get());
		deleteOperation.Release();
		return SmartPointer<POINTED_TYPE>(pointer.Release(), manager.Release());
	}
	template<typename ...ARGS>
	StdPointer(ARGS&&... args) :
		SmartPointer<POINTED_TYPE>(Create(std::forward<ARGS>(args)...)) {
	}
};

template<typename OBJECT_TYPE>
class Scoped {
private:
	OBJECT_TYPE m_Object;

public:
	template<typename ...ARGS>
	Scoped(ARGS&&... args) :
		m_Object(std::forward<ARGS>(args)...) {
	}

	SmartPointer<OBJECT_TYPE> operator &() {
		return SmartPointer<OBJECT_TYPE>(&m_Object, &DummyManager);
	}

	OBJECT_TYPE* operator ->() {
		return &m_Object;
	}
};

#endif /* PROJECT_INCLUDE_PREDEFINED_SMART_POINTERS_H_ */
