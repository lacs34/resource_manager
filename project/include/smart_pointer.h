/*
 * smart_pointer.h
 *
 *  Created on: 2016Äê3ÔÂ14ÈÕ
 *      Author: Administrator
 */

#ifndef PROJECT_INCLUDE_SMART_POINTER_H_
#define PROJECT_INCLUDE_SMART_POINTER_H_

#include "memory_manager_interface.h"

template<typename ...RESOURCE_PASSING_TYPES>
class ResourcePassTypeList {
};

template<typename RESOURCE_TYPE, RESOURCE_TYPE DEFAULT_VALUE>
class ValueResourceContainer {
private:
	RESOURCE_TYPE m_Resource;

public:
	typedef RESOURCE_TYPE OUT_PASSING_TYPE;
	typedef ResourcePassTypeList<RESOURCE_TYPE> IN_PASSING_TYPES;
	inline ValueResourceContainer(RESOURCE_TYPE resource) :
		m_Resource(resource) {
	}
	inline ValueResourceContainer(ValueResourceContainer<RESOURCE_TYPE, DEFAULT_VALUE> &resource) :
		m_Resource(resource.m_Resource) {
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

template<typename RESOURCE_CONTAINER>
class TResourceHolder;

template<typename RESOURCE_CONTAINER, typename IN_PASSING_TYPE_HEADER, typename ...RESOURCE_PASSING_TYPES>
class TResourceHolderBase :
    public TResourceHolderBase<RESOURCE_CONTAINER, RESOURCE_PASSING_TYPES...> {
public:
	using TResourceHolderBase<RESOURCE_CONTAINER, RESOURCE_PASSING_TYPES...>::TResourceHolderBase;
	TResourceHolderBase(IN_PASSING_TYPE_HEADER resource, ResourceManager *&manager) :
		m_Resource(std::forward<IN_PASSING_TYPE_HEADER>(resource)), m_Manager(manager) {
	}
	TResourceHolderBase(IN_PASSING_TYPE_HEADER resource, ResourceManager *&&manager) :
		m_Resource(std::forward<IN_PASSING_TYPE_HEADER>(resource)), m_Manager(std::move(manager)) {
	}
	
	inline void Attach(IN_PASSING_TYPE_HEADER resource, ResourceManager *&manager) {
		m_Resource.SetResource(resource);
		m_Manager = manager;
	}
	inline void Attach(IN_PASSING_TYPE_HEADER resource, ResourceManager *&&manager) {
		m_Resource.SetResource(resource);
		m_Manager = std::move(manager);
	}
};

template<typename RESOURCE_CONTAINER, typename IN_PASSING_TYPE>
class TResourceHolderBase<RESOURCE_CONTAINER, IN_PASSING_TYPE> {
private:
	RESOURCE_CONTAINER m_Resource;
	ManagerHolder m_Manager;

public:
	TResourceHolderBase(IN_PASSING_TYPE resource, ResourceManager *&manager) :
		m_Resource(std::forward<IN_PASSING_TYPE>(resource)), m_Manager(manager) {
	}
	TResourceHolderBase(IN_PASSING_TYPE resource, ResourceManager *&&manager) :
		m_Resource(std::forward<IN_PASSING_TYPE>(resource)), m_Manager(std::move(manager)) {
	}

	inline void Attach(IN_PASSING_TYPE resource, ResourceManager *&manager) {
		m_Resource.SetResource(std::forward<IN_PASSING_TYPE>(resource));
		m_Manager = manager;
	}
	inline void Attach(IN_PASSING_TYPE resource, ResourceManager *&&manager) {
		m_Resource.SetResource(std::forward<IN_PASSING_TYPE>(resource));
		m_Manager = std::move(manager);
	}
};

template<typename RESOURCE_CONTAINER, typename IN_PASSING_TYPE>
class TypeListToHolderBaseHelper {
public:
	typedef TResourceHolderBase<IN_PASSING_TYPE, IN_PASSING_TYPE> HOLDER_BASE;
};

template<typename RESOURCE_CONTAINER, typename ...IN_PASSING_TYPES>
class TypeListToHolderBaseHelper<RESOURCE_CONTAINER, ResourcePassTypeList<IN_PASSING_TYPES...>> {
public:
	typedef TResourceHolderBase<RESOURCE_CONTAINER, IN_PASSING_TYPES...> HOLDER_BASE;
};

template<typename RESOURCE_CONTAINER>
class TResourceHolder :
	public TypeListToHolderBaseHelper<RESOURCE_CONTAINER, typename RESOURCE_CONTAINER::IN_PASSING_TYPES>::HOLDER_BASE {
private:
	RESOURCE_CONTAINER m_Resource;
	ManagerHolder m_Manager;
	typedef typename RESOURCE_CONTAINER::RESOURCE_PASSING_TYPE RESOURCE_TYPE;

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
	TResourceHolder(RESOURCE_TYPE resource, ResourceManager *&manager) :
		m_Resource(resource), m_Manager(manager) {
	}
	TResourceHolder(RESOURCE_TYPE resource, ResourceManager *&&manager) :
		m_Resource(resource), m_Manager(std::move(manager)) {
	}
	TResourceHolder(const TResourceHolder<RESOURCE_CONTAINER> &resource) :
		TResourceHolder(pointer.m_Pointer, pointer.m_Manager) {
	}
	TResourceHolder(TResourceHolder<RESOURCE_CONTAINER> &&resource) :
		TResourceHolder(pointer.m_Pointer, std::move(pointer.m_Manager)) {
	}

	inline RESOURCE_TYPE GetResource() {
		return m_Resource;
	}

	inline operator RESOURCE_TYPE() {
		return m_Resource;
	}

	inline void Attach(const TResourceHolder<RESOURCE_TYPE> &holder) {
		ResourceManager *manager = holder.m_Manager;
		manager->IncreaseReference();
		Clear();
		m_Pointer = holder.m_Pointer;
		m_Manager = manager;
		return *this;
	}
	inline void Attach(TResourceHolder<RESOURCE_TYPE> &&holder) {
		Clear();
		m_Resource.SetResource(holder.GetResource());
		m_Manager = resource.m_Manager;
		holder.Clear();
	}
	inline void Attach(RESOURCE_TYPE resource, ResourceManager *&manager) {
		m_Resource.SetResource(resource);
		m_Manager = manager;
	}
	inline void Attach(RESOURCE_TYPE resource, ResourceManager *&&manager) {
		m_Resource.SetResource(resource);
		m_Manager = std::move(manager);
	}

	inline void Detach() {
		m_Resource.ResetToDefault();
		m_Manager.Detach();
	}

	inline TResourceHolder<RESOURCE_TYPE>& operator =(const TResourceHolder<RESOURCE_TYPE> &resource) {
		Attach(resource);
		return *this;
	}

	inline TResourceHolder<RESOURCE_TYPE>& operator =(TResourceHolder<RESOURCE_TYPE> &&resource) {
		Attach(std::move(resource));
		return *this;
	}

	virtual ~TResourceHolder() {
		if (m_Manager != nullptr) {
			m_Manager->DecreaseReference();
		}
	}
};

template<typename POINTED_TYPE>
class PointerResourceAdapter :
	public ResourceHolder<PointerResourceContainer<POINTED_TYPE>> {
private:
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
	SmartPointer(POINTED_TYPE *pointer, ResourceManager *manager) :
		m_Pointer(pointer), m_Manager(manager) {
	}
	SmartPointer(const SmartPointer<POINTED_TYPE> &pointer) :
		SmartPointer(pointer.m_Pointer, pointer.m_Manager) {
		if (m_Manager != nullptr) {
			m_Manager->IncreaseReference();
		}
	}
	SmartPointer(SmartPointer<POINTED_TYPE> &&pointer) :
		SmartPointer(pointer.m_Pointer, pointer.m_Manager) {
		pointer.Clear();
	}
	POINTED_TYPE* operator ->() {
		return m_Pointer;
	}
	const POINTED_TYPE* operator ->() const {
		return m_Pointer;
	}
	POINTED_TYPE* GetPointer() const {
		return m_Pointer;
	}
	SmartPointer<POINTED_TYPE>& operator =(const SmartPointer<POINTED_TYPE> &pointer) {
		if (EqualTo(pointer)) {
			return *this;
		}
		ResourceManager *manager = pointer.m_Manager;
		manager->IncreaseReference();
		Clear();
		m_Pointer = pointer.m_Pointer;
		m_Manager = manager;
		return *this;
	}
	template<typename DEST_TYPE>
	SmartPointer<DEST_TYPE> Cast() {
		if (m_Manager != nullptr) {
			m_Manager->IncreaseReference();
		}
		return SmartPointer<DEST_TYPE>(static_cast<DEST_TYPE*>(m_Pointer), m_Manager);
	}
	~SmartPointer() {
		Release();
	}
};

#endif /* PROJECT_INCLUDE_SMART_POINTER_H_ */
