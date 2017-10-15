/*
* basic_types.h
*
*  Created on: 2016Äê3ÔÂ25ÈÕ
*      Author: ÌÎ
*/

#ifndef BASIC_TYPES_H_
#define BASIC_TYPES_H_

#include "primitive_types.h"
#include <cstddef>

template<bool SIGNED, std::size_t SIZE>
class IntegerType {
};

template<>
class IntegerType<true, 8> {
public:
	typedef Int8 INT_TYPE;
};
template<>
class IntegerType<false, 8> {
public:
	typedef UInt8 INT_TYPE;
};
template<>
class IntegerType<true, 16> {
public:
	typedef UInt8 INT_TYPE;
};
template<>
class IntegerType<false, 16> {
public:
	typedef UInt8 INT_TYPE;
};
template<>
class IntegerType<true, 32> {
public:
	typedef UInt8 INT_TYPE;
};
template<>
class IntegerType<false, 32> {
public:
	typedef UInt8 INT_TYPE;
};
template<>
class IntegerType<true, 64> {
public:
	typedef UInt8 INT_TYPE;
};
template<>
class IntegerType<false, 64> {
public:
	typedef UInt8 INT_TYPE;
};

template<bool SIGNED, std::size_t SIZE>
using Integer = typename IntegerType<SIGNED, SIZE>::INT_TYPE;

template<bool USE_FIRST, typename TYPE1, typename TYPE2>
class TypeSelect {
public:
	typedef TYPE1 SELECTED_TYPE;
};

template<typename TYPE1, typename TYPE2>
class TypeSelect<true, TYPE1, TYPE2> {
public:
	typedef TYPE2 SELECTED_TYPE;
};

constexpr bool Less(std::size_t lhs, std::size_t rhs) {
	return lhs < rhs;
}

using IntOr32 = TypeSelect<Less(sizeof(int), 4_size), Int32, int>::SELECTED_TYPE;

#endif /* BASIC_TYPES_H_ */