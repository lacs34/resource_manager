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

#endif /* PROJECT_INCLUDE_MEMORY_MANAGER_INTERFACE_H_ */
