/*
 * smart_pointer.h
 *
 *  Created on: 2016Äê3ÔÂ14ÈÕ
 *      Author: Administrator
 */

#ifndef PROJECT_INCLUDE_SMART_POINTER_H_
#define PROJECT_INCLUDE_SMART_POINTER_H_

#include "memory_manager_interface.h"

template<typename RESOURCE_TYPE>
class CommonResourceContainer {
private:
	RESOURCE_TYPE m_Resource;

public:
	typedef RESOURCE_TYPE& OUT_PASSING_TYPE;
	template<typename ...CONSTRUCTOR_ARGUMENTS>
	inline CommonResourceContainer(CONSTRUCTOR_ARGUMENTS &&...arguments) :
		m_Resource(std::forward<CONSTRUCTOR_ARGUMENTS>(arguments)...) {
	}
	inline CommonResourceContainer(CommonResourceContainer<RESOURCE_TYPE> &container) :
		m_Resource(container.m_Resource) {
	}
	inline CommonResourceContainer(CommonResourceContainer<RESOURCE_TYPE> &&container) :
		m_Resource(std::move(container.m_Resource)) {
	}

	inline OUT_PASSING_TYPE GetResource() {
		return m_Resource;
	}
	inline void SetResource(RESOURCE_TYPE &resource) {
		m_Resource = resource;
	}
	inline void SetResource(RESOURCE_TYPE &&resource) {
		m_Resource = std::move(resource);
	}

	inline bool IsSameResource(RESOURCE_TYPE &resource) {
		return m_Resource == resource;
	}
	inline bool IsSameResource(RESOURCE_TYPE &&resource) {
		return m_Resource == std::move(resource);
	}
	inline void ResetToDefault() {
	}
};

template<typename RESOURCE_TYPE, RESOURCE_TYPE DEFAULT_VALUE>
class ValueResourceContainer {
private:
	RESOURCE_TYPE m_Resource;

public:
	typedef RESOURCE_TYPE OUT_PASSING_TYPE;
	inline ValueResourceContainer(const ValueResourceContainer<RESOURCE_TYPE, DEFAULT_VALUE> &resource) :
		m_Resource(resource.m_Resource) {
	}
	inline ValueResourceContainer(ValueResourceContainer<RESOURCE_TYPE, DEFAULT_VALUE> &&resource) :
		m_Resource(resource.m_Resource) {
		resource.ResetToDefault();
	}
	inline ValueResourceContainer(RESOURCE_TYPE resource) :
		m_Resource(resource) {
	}

	inline RESOURCE_TYPE GetResource() {
		return m_Resource;
	}
	inline void SetResource(RESOURCE_TYPE resource) {
		m_Resource = resource;
	}

	inline bool IsSameResource(RESOURCE_TYPE resource) {
		return m_Resource == resource;
	}
	inline void ResetToDefault() {
		m_Resource = DEFAULT_VALUE;
	}
};

template<typename POINTED_TYPE>
class CommonResourceContainer<POINTED_TYPE*> :
    public ValueResourceContainer<POINTED_TYPE*, nullptr> {
public:
	using ValueResourceContainer<POINTED_TYPE*, nullptr>::ValueResourceContainer;
};

template<>
class CommonResourceContainer<signed char> :
	public ValueResourceContainer<signed char, (signed char)0> {
public:
	using ValueResourceContainer<signed char, (signed char)0>::ValueResourceContainer;
};
template<>
class CommonResourceContainer<unsigned char> :
	public ValueResourceContainer<unsigned char, (unsigned char)0> {
public:
	using ValueResourceContainer<unsigned char, (unsigned char)0>::ValueResourceContainer;
};

template<>
class CommonResourceContainer<short> :
	public ValueResourceContainer<short, (short)0> {
public:
	using ValueResourceContainer<short, (short)0>::ValueResourceContainer;
};
template<>
class CommonResourceContainer<unsigned short> :
	public ValueResourceContainer<unsigned short, (unsigned short)0> {
public:
	using ValueResourceContainer<unsigned short, (unsigned short)0>::ValueResourceContainer;
};

template<>
class CommonResourceContainer<int> :
	public ValueResourceContainer<int, 0> {
public:
	using ValueResourceContainer<int, 0>::ValueResourceContainer;
};
template<>
class CommonResourceContainer<unsigned> :
	public ValueResourceContainer<unsigned, 0U> {
public:
	using ValueResourceContainer<unsigned, 0U>::ValueResourceContainer;
};

template<>
class CommonResourceContainer<long> :
	public ValueResourceContainer<long, 0L> {
public:
	using ValueResourceContainer<long, 0L>::ValueResourceContainer;
};
template<>
class CommonResourceContainer<unsigned long> :
	public ValueResourceContainer<unsigned long, 0UL> {
public:
	using ValueResourceContainer<unsigned long, 0UL>::ValueResourceContainer;
};

template<>
class CommonResourceContainer<long long> :
	public ValueResourceContainer<long long, 0LL> {
public:
	using ValueResourceContainer<long long, 0LL>::ValueResourceContainer;
};
template<>
class CommonResourceContainer<unsigned long long> :
	public ValueResourceContainer<unsigned long long, 0ULL> {
public:
	using ValueResourceContainer<unsigned long long, 0ULL>::ValueResourceContainer;
};

template<typename RESOURCE_CONTAINER>
class TResourceHolder {
private:
	RESOURCE_CONTAINER m_Resource;
	ManagerHolder m_Manager;
	typedef typename RESOURCE_CONTAINER::OUT_PASSING_TYPE RESOURCE_TYPE;

private:
	void Clear() {
		m_Resource.ResetToDefault();
		m_Manager = nullptr;
	}
	void Release() {
		if (m_Manager != nullptr) {
			m_Manager->DecreaseReference();
			Clear();
		}
	}

public:
	template<typename ...RESOURCE_CONSTRUCT_TYPES>
	TResourceHolder(ResourceManager &manager, RESOURCE_CONSTRUCT_TYPES &&...args) :
		m_Resource(std::forward<RESOURCE_CONSTRUCT_TYPES>(args)...), m_Manager(manager) {
	}
	template<typename ...RESOURCE_CONSTRUCT_TYPES>
	TResourceHolder(ResourceManager &&manager, RESOURCE_CONSTRUCT_TYPES &&...args) :
		m_Resource(std::forward<RESOURCE_CONSTRUCT_TYPES>(args)...), m_Manager(std::move(manager)) {
	}
	TResourceHolder(const TResourceHolder<RESOURCE_CONTAINER> &resource) :
		m_Resource(resource.m_Resource), m_Manager(resource.m_Manager) {
	}
	TResourceHolder(TResourceHolder<RESOURCE_CONTAINER> &&resource) :
		m_Resource(std::move(resource.m_Resource)), m_Manager(std::move(resource.m_Manager)) {
	}

	inline RESOURCE_TYPE GetResource() {
		return m_Resource.GetResource();
	}

	inline ResourceManager& GetManager() const {
		return m_Manager.GetManager();
	}

	inline operator RESOURCE_TYPE() {
		return m_Resource.GetResource();
	}

	inline void Attach(const TResourceHolder<RESOURCE_CONTAINER> &holder) {
		m_Resource.SetResource(holder.m_Resource);
		m_Manager = holder.manager;
	}
	inline void Attach(TResourceHolder<RESOURCE_CONTAINER> &&holder) {
		m_Resource.SetResource(std::move(holder.m_Resource));
		m_Manager = std::move(holder.m_Manager);
	}
	template<typename ...RESOURCE_ATTACH_TYPES>
	inline void Attach(ResourceManager &manager, RESOURCE_ATTACH_TYPES &&...args) {
		m_Resource.SetResource(std::forward<RESOURCE_ATTACH_TYPES>(args)...);
		m_Manager = manager;
	}
	template<typename ...RESOURCE_ATTACH_TYPES>
	inline void Attach(ResourceManager &&manager, RESOURCE_ATTACH_TYPES &&...args) {
		m_Resource.SetResource(std::forward<RESOURCE_ATTACH_TYPES>(args)...);
		m_Manager = std::move(manager);
	}

	inline void Detach() {
		m_Resource.ResetToDefault();
		m_Manager.Detach();
	}

	inline TResourceHolder<RESOURCE_CONTAINER>& operator =(const TResourceHolder<RESOURCE_CONTAINER> &resource) {
		Attach(resource);
		return *this;
	}

	inline TResourceHolder<RESOURCE_CONTAINER>& operator =(TResourceHolder<RESOURCE_CONTAINER> &&resource) {
		Attach(std::move(resource));
		return *this;
	}
};

template<typename RESOURCE_TYPE>
using ResourceHolder = TResourceHolder<CommonResourceContainer<RESOURCE_TYPE>>;

template<typename POINTED_TYPE>
class PointerResourceAdapter :
	public TResourceHolder<ValueResourceContainer<POINTED_TYPE*, nullptr>> {
public:
	using TResourceHolder<ValueResourceContainer<POINTED_TYPE*, nullptr>>::TResourceHolder;

	inline POINTED_TYPE* GetPointer() {
		return GetResource();
	}
};

template<typename POINTED_TYPE>
class SmartPointer :
    public PointerCommonOperation<POINTED_TYPE, SmartPointer<POINTED_TYPE>, PointerResourceAdapter<POINTED_TYPE>> {
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
	using PointerCommonOperation<POINTED_TYPE, SmartPointer<POINTED_TYPE>, PointerResourceAdapter<POINTED_TYPE>>::PointerCommonOperation;

	SmartPointer(const SmartPointer<POINTED_TYPE> &pointer) :
		PointerCommonOperation(pointer) {
	}
	SmartPointer(SmartPointer<POINTED_TYPE> &&pointer) :
		PointerCommonOperation(std::move(pointer)) {
	}
	SmartPointer<POINTED_TYPE>& operator =(const SmartPointer<POINTED_TYPE> &pointer) {
		if (EqualTo(pointer)) {
			return *this;
		}
		Attach(pointer);
		return *this;
	}
	SmartPointer<POINTED_TYPE>& operator =(const SmartPointer<POINTED_TYPE> &&pointer) {
		if (EqualTo(pointer)) {
			return *this;
		}
		Attach(std::move(pointer));
		return *this;
	}
	template<typename DEST_TYPE>
	SmartPointer<DEST_TYPE> Cast() {
		if (&GetManager() != nullptr) {
			GetManager().IncreaseReference();
		}
		SmartPointer<DEST_TYPE> destPointer(GetManager(), static_cast<DEST_TYPE*>(GetPointer()));
		return destPointer;
	}
};

#endif /* PROJECT_INCLUDE_SMART_POINTER_H_ */
