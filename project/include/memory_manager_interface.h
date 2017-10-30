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
	manager->IncreaseReference();
	return manager;
}

template<typename POINTED_TYPE, typename POINTER_WRAPPER, typename BASE>
class PointerCommonOperation :
    public BASE {
public:
	friend POINTER_WRAPPER;
	using BASE::BASE;

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

	inline POINTED_TYPE* GetPointer() const {
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
	inline ManagerHolder(ResourceManager &manager) :
		PointerCommonOperation(IncreaseAndReturn(&manager)) {
	}
	inline ManagerHolder(ResourceManager &&manager) :
		PointerCommonOperation(&manager) {
	}
	inline ManagerHolder(const ManagerHolder &holder) :
		PointerCommonOperation(IncreaseAndReturn(holder.m_Pointer)) {
	}
	inline ManagerHolder(ManagerHolder &&holder) :
		PointerCommonOperation(holder.Detach()) {
	}

	ResourceManager& GetManager() const {
		return *m_Pointer;
	}

	inline void Attach(ResourceManager &manager) {
		ManagerHolder holder(manager);
		Release();
		m_Pointer = holder.Detach();
	}

	inline void Attach(ResourceManager &&manager) {
		Release();
		m_Pointer = &manager;
	}

	inline void Attach(ManagerHolder &holder) {
		Release();
		m_Pointer = IncreaseAndReturn(holder.m_Pointer);
	}

	inline void Attach(ManagerHolder &&holder) {
		Release();
		m_Pointer = holder.Detach();
	}

	inline ResourceManager* Detach() {
		ResourceManager *manager = m_Pointer;
		m_Pointer = nullptr;
		return manager;
	}

	inline ManagerHolder& operator =(ResourceManager &manager) {
		Attach(manager);
		return *this;
	}

	inline ManagerHolder& operator =(ResourceManager &&manager) {
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
