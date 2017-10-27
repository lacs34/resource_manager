/*
 * memory_manager_interface.h
 *
 *  Created on: 2016Äê3ÔÂ13ÈÕ
 *      Author: Administrator
 */

#ifndef PROJECT_INCLUDE_MEMORY_MANAGER_INTERFACE_H_
#define PROJECT_INCLUDE_MEMORY_MANAGER_INTERFACE_H_

#include <utility>

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

template<typename POINTED_TYPE, typename POINTER_WRAPPER, typename BASE>
class PointerCommonOperation :
    public BASE {
public:
	friend POINTER_WRAPPER;

	inline PointerCommonOperation(POINTED_TYPE *pointer) :
		BASE(pointer) {
	}
	inline PointerCommonOperation(const POINTED_TYPE *pointer) :
		BASE(pointer) {
	}

	inline POINTED_TYPE* operator ->() {
		return GetPointer();
	}
	inline const POINTED_TYPE* operator ->() const {
		return GetPointer();
	}

	inline operator POINTED_TYPE*() {
		return GetPointer();
	}

	inline bool operator ==(POINTED_TYPE *another) const {
		return GetPointer() == another;
	}
	inline bool operator ==(POINTER_WRAPPER &another) const {
		return GetPointer() == another.GetPointer();
	}
	inline bool operator ==(POINTER_WRAPPER &&another) const {
		return GetPointer() == another.GetPointer();
	}

	inline bool operator !=(POINTED_TYPE *another) const {
		return GetPointer() != another;
	}
	inline bool operator !=(POINTER_WRAPPER &another) const {
		return GetPointer() != another.GetPointer();
	}
	inline bool operator !=(POINTER_WRAPPER &&another) const {
		return GetPointer() != another.GetPointer();
	}

	inline bool operator <(POINTED_TYPE *another) const {
		return GetPointer() < another;
	}
	inline bool operator <(POINTER_WRAPPER &another) const {
		return GetPointer() < another.GetPointer();
	}
	inline bool operator <(POINTER_WRAPPER &&another) const {
		return GetPointer() < another.GetPointer();
	}

	inline bool operator <=(POINTED_TYPE *another) const {
		return GetPointer() <= another;
	}
	inline bool operator <=(POINTER_WRAPPER &another) const {
		return GetPointer() <= another.GetPointer();
	}
	inline bool operator <=(POINTER_WRAPPER &&another) const {
		return GetPointer() <= another.GetPointer();
	}

	inline bool operator >(POINTED_TYPE *another) const {
		return GetPointer() > another;
	}
	inline bool operator >(POINTER_WRAPPER &another) const {
		return GetPointer() > another.GetPointer();
	}
	inline bool operator >(POINTER_WRAPPER &&another) const {
		return GetPointer() > another.GetPointer();
	}

	inline bool operator >=(POINTED_TYPE *another) const {
		return GetPointer() >= another;
	}
	inline bool operator >=(POINTER_WRAPPER &another) const {
		return GetPointer() >= another.GetPointer();
	}
	inline bool operator >=(POINTER_WRAPPER &&another) const {
		return GetPointer() >= another.GetPointer();
	}
};

template<typename POINTED_TYPE, typename POINTER_WRAPPER>
class SimplePointerOperationBase {
private:
	POINTED_TYPE *m_Pointer;

	inline POINTED_TYPE* GetPointer() {
		return m_Pointer;
	}

public:
	friend PointerCommonOperation<POINTED_TYPE, POINTER_WRAPPER, SimplePointerOperationBase<POINTED_TYPE, POINTER_WRAPPER>>;
	friend POINTER_WRAPPER;

	inline SimplePointerOperationBase(POINTED_TYPE *pointer) :
		m_Pointer(pointer) {
	}
	inline SimplePointerOperationBase(const POINTED_TYPE *pointer) :
		m_Pointer(pointer) {
	}
	inline SimplePointerOperationBase() :
		m_Pointer(nullptr) {
	}
};

class ManagerHolder :
    public PointerCommonOperation<ResourceManager, ManagerHolder, SimplePointerOperationBase<ResourceManager, ManagerHolder>> {
private:
	void Release() {
		if (m_Pointer != nullptr) {
			m_Pointer->DecreaseReference();
			m_Pointer = nullptr;
		}
	}

public:
	inline ManagerHolder(ResourceManager *&manager) :
		PointerCommonOperation(IncreaseAndReturn(manager)) {
	}
	inline ManagerHolder(ResourceManager *&&manager) :
		PointerCommonOperation(ClearAndReturn(std::move(manager))) {
	}
	inline ManagerHolder(ManagerHolder &holder) :
		PointerCommonOperation(IncreaseAndReturn(holder.m_Pointer)) {
	}
	inline ManagerHolder(ManagerHolder &&holder) :
		PointerCommonOperation(ClearAndReturn(std::move(holder.m_Pointer))) {
	}

	inline void Attach(ResourceManager *&manager) {
		ManagerHolder holder(manager);
		Release();
		m_Pointer = ClearAndReturn(std::move(holder.m_Pointer));
	}

	inline void Attach(ResourceManager *&&manager) {
		ManagerHolder holder(std::move(manager));
		Release();
		m_Pointer = ClearAndReturn(std::move(holder.m_Pointer));
	}

	inline void Attach(ManagerHolder &manager) {
		ManagerHolder holder(manager);
		Release();
		m_Pointer = ClearAndReturn(std::move(holder.m_Pointer));
	}

	inline void Attach(ManagerHolder &&manager) {
		ManagerHolder holder(std::move(manager));
		Release();
		m_Pointer = ClearAndReturn(std::move(holder.m_Pointer));
	}

	inline ResourceManager* Detach() {
		ResourceManager *manager = m_Pointer;
		m_Pointer = nullptr;
		return manager;
	}

	inline ManagerHolder& operator =(ResourceManager *&manager) {
		Attach(manager);
		return *this;
	}

	inline ManagerHolder& operator =(ResourceManager *&&manager) {
		Attach(std::move(manager));
		return *this;
	}

	inline ManagerHolder& operator =(ManagerHolder &manager) {
		Attach(manager);
		return *this;
	}

	inline ManagerHolder& operator =(ManagerHolder &&manager) {
		Attach(std::move(manager));
		return *this;
	}

	inline ~ManagerHolder() {
		if (m_Pointer != nullptr) {
			m_Pointer->DecreaseReference();
		}
	}
};

#endif /* PROJECT_INCLUDE_MEMORY_MANAGER_INTERFACE_H_ */
