/*
 * collections_interfaces.h
 *
 *  Created on: 2016Äê3ÔÂ25ÈÕ
 *      Author: ÌÎ
 */

#ifndef PROJECT_INCLUDE_COLLECTIONS_COLLECTIONS_INTERFACES_H_
#define PROJECT_INCLUDE_COLLECTIONS_COLLECTIONS_INTERFACES_H_

#include "smart_pointer.h"

template<typename ELEMENT_TYPE>
class Iterator {
public:
	virtual bool MoveNext() = 0;
	virtual ELEMENT_TYPE GetCurrent() = 0;
	virtual ~Iterator() { }
};

template<typename ELEMENT_TYPE>
class Iteratable {
public:
	virtual SmartPointer<Iterator<ELEMENT_TYPE>> GetIterator() = 0;
	virtual ~Iteratable() { }
};

template<typename ELEMENT_TYPE>
class ReadOnlyCollection :
	public Iteratable<ELEMENT_TYPE> {
public:
	virtual size_t GetCount() = 0;
	virtual ~ReadOnlyCollection() { }
};

template<typename ELEMENT_TYPE>
class Collection :
	public Iteratable<ELEMENT_TYPE> {
public:
	virtual size_t GetCount() = 0;
	virtual void Add(const ELEMENT_TYPE &element) = 0;
	virtual void Clear() = 0;
	virtual ~Collection() { }
};

template<typename ELEMENT_TYPE>
class ReadOnlyList :
	public ReadOnlyCollection<ELEMENT_TYPE> {
public:
	virtual ELEMENT_TYPE GetAt(size_t index) = 0;
	virtual ~ReadOnlyList() { }
};

template<typename ELEMENT_TYPE>
class List :
	public Collection<ELEMENT_TYPE> {
public:
	virtual ELEMENT_TYPE GetAt(size_t index) = 0;
	virtual void SetAt(size_t index, const ELEMENT_TYPE &element) = 0;
	virtual void InsertAt(size_t index, const ELEMENT_TYPE &element) = 0;
	virtual void RemoveAt(size_t index) = 0;
	virtual ~List() { }
};

#endif /* PROJECT_INCLUDE_COLLECTIONS_COLLECTIONS_INTERFACES_H_ */
