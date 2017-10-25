/*
 * memory_manager_interface.h
 *
 *  Created on: 2016Äê3ÔÂ13ÈÕ
 *      Author: Administrator
 */

#ifndef PROJECT_INCLUDE_MEMORY_MANAGER_INTERFACE_H_
#define PROJECT_INCLUDE_MEMORY_MANAGER_INTERFACE_H_

class MemoryManager {
public:
	virtual void IncreaseReference() = 0;
	virtual void DecreaseReference() = 0;
	virtual ~MemoryManager() { }
};

class DeleteOperation {
public:
	virtual void DoDelete() = 0;
	virtual ~DeleteOperation() { }
};

class ManagerHolder {
private:
	MemoryManager *m_Manager;

public:
	inline ManagerHolder(MemoryManager *manager) :
	    m_Manager(manager) {
		if (manager != nullptr) {
			manager->IncreaseReference();
		}
	}

	inline MemoryManager* operator ->() {
		return m_Manager;
	}

	inline operator MemoryManager*() {
		return m_Manager;
	}

	inline ~ManagerHolder() {
		if (m_Manager != nullptr) {
			m_Manager->DecreaseReference();
		}
	}
};

#endif /* PROJECT_INCLUDE_MEMORY_MANAGER_INTERFACE_H_ */
