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
class ResourceHolder {
private:
	RESOURCE_TYPE m_Resource;
	ManagerHolder m_Manager;

private:
	void Clear() {
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
	ResourceHolder(RESOURCE_TYPE resource, ResourceManager *&manager) :
		m_Resource(resource), m_Manager(manager) {
	}
	ResourceHolder(RESOURCE_TYPE resource, ResourceManager *&&manager) :
		m_Resource(resource), m_Manager(std::move(manager)) {
	}
	ResourceHolder(const ResourceHolder<RESOURCE_TYPE> &resource) :
		ResourceHolder(pointer.m_Pointer, pointer.m_Manager) {
	}
	ResourceHolder(ResourceHolder<RESOURCE_TYPE> &&resource) :
		ResourceHolder(pointer.m_Pointer, std::move(pointer.m_Manager)) {
	}

	inline RESOURCE_TYPE& GetResource() {
		return m_Resource;
	}

	inline operator RESOURCE_TYPE&() {
		return m_Resource;
	}

	SmartPointer<POINTED_TYPE>& operator =(const SmartPointer<POINTED_TYPE> &pointer) {
		ResourceManager *manager = pointer.m_Manager;
		manager->IncreaseReference();
		Clear();
		m_Pointer = pointer.m_Pointer;
		m_Manager = manager;
		return *this;
	}
};

template<typename POINTED_TYPE>
class SmartPointer :
    public ResourceHolder<POINTED_TYPE*> {

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
