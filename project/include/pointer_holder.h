/*
 * pointer_holder.h
 *
 *  Created on: 2016Äê7ÔÂ5ÈÕ
 *      Author: ÌÎ
 */

#ifndef PROJECT_INCLUDE_POINTER_HOLDER_H_
#define PROJECT_INCLUDE_POINTER_HOLDER_H_

template<typename POINTED_TYPE>
class PointerHolder {
private:
	POINTED_TYPE *m_Pointer;
	DeleteOperation *m_DeleteOperation;

public:
	PointerHolder(POINTED_TYPE *pointer, DeleteOperation *deleteOperation) :
		m_Pointer(pointer), m_DeleteOperation(deleteOperation) {
	}
	PointerHolder(const PointerHolder &holder) :
		m_Pointer(holder.m_Pointer), m_DeleteOperation(holder.m_DeleteOperation) {
	}

	PointerHolder<POINTED_TYPE>& operator =(const PointerHolder<POINTED_TYPE> &holder) {
		m_Pointer = holder.m_Pointer;
		m_DeleteOperation = holder.m_DeleteOperation;
		return *this;
	}

	POINTED_TYPE* operator ->() {
		return m_Pointer;
	}
	const POINTED_TYPE* operator ->() const {
		return m_Pointer;
	}

	void DeletePointedObject() {
		m_DeleteOperation->DoDelete();
	}
};



#endif /* PROJECT_INCLUDE_POINTER_HOLDER_H_ */
