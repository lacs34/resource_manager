/*
 * memory_manager_interface.h
 *
 *  Created on: 2016Äê3ÔÂ13ÈÕ
 *      Author: Administrator
 */

#ifndef PROJECT_INCLUDE_MEMORY_MANAGER_INTERFACE_H_
#define PROJECT_INCLUDE_MEMORY_MANAGER_INTERFACE_H_

class ResourceManager {
public:
	virtual void IncreaseReference() = 0;
	virtual void DecreaseReference() = 0;
	virtual ~ResourceManager() { }
};

class DeleteOperation {
public:
	virtual void DoDelete() = 0;
	virtual ~DeleteOperation() { }
};

inline ResourceManager* IncreaseAndReturn(ResourceManager *manager) {
	if (manager != nullptr) {
		manager->IncreaseReference();
	}
	return manager;
}

inline ResourceManager* ClearAndReturn(ResourceManager *&&manager) {
	ResourceManager *copiedManager = manager;
	manager = nullptr;
	return copiedManager;
}

class ManagerHolder {
private:
	ResourceManager *m_Manager;
	void Release() {
		if (m_Manager != nullptr) {
			m_Manager->DecreaseReference();
			m_Manager = nullptr;
		}
	}

public:
	inline ManagerHolder(ResourceManager *&manager) :
	    m_Manager(IncreaseAndReturn(manager)) {
	}
	inline ManagerHolder(ResourceManager *&&manager) :
		m_Manager(ClearAndReturn(std::move(manager))) {
	}
	inline ManagerHolder(ManagerHolder &holder) :
		m_Manager(IncreaseAndReturn(holder.m_Manager)) {
	}
	inline ManagerHolder(ManagerHolder &&holder) :
		m_Manager(ClearAndReturn(std::move(holder.m_Manager))) {
	}

	inline ResourceManager* operator ->() {
		return m_Manager;
	}

	inline operator ResourceManager*() {
		return m_Manager;
	}

	inline ManagerHolder& operator =(ResourceManager *&manager) {
		ManagerHolder holder(manager);
		Release();
		m_Manager = ClearAndReturn(std::move(holder.m_Manager));
		return *this;
	}

	inline ManagerHolder& operator =(ResourceManager *&&manager) {
		ManagerHolder holder(std::move(manager));
		Release();
		m_Manager = ClearAndReturn(std::move(holder.m_Manager));
		return *this;
	}

	inline ManagerHolder& operator =(ManagerHolder &manager) {
		ManagerHolder holder(manager);
		Release();
		m_Manager = ClearAndReturn(std::move(holder.m_Manager));
		return *this;
	}

	inline ManagerHolder& operator =(ManagerHolder &&manager) {
		ManagerHolder holder(std::move(manager));
		Release();
		m_Manager = ClearAndReturn(std::move(holder.m_Manager));
		return *this;
	}

	inline ~ManagerHolder() {
		if (m_Manager != nullptr) {
			m_Manager->DecreaseReference();
		}
	}
};

#endif /* PROJECT_INCLUDE_MEMORY_MANAGER_INTERFACE_H_ */
