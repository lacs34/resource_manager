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

template<typename INT_TYPE>
constexpr bool IsSigned() {
	return INT_TYPE(INT_TYPE(0) - INT_TYPE(1)) < INT_TYPE(0);
}

template<typename INT_TYPE>
constexpr INT_TYPE MaxValueHighestByte() {
	return IsSigned<INT_TYPE>() ? INT_TYPE(0x7F) : INT_TYPE(0xFF);
}
template<std::size_t UNINITIALIZED_SIZE, typename INT_TYPE>
class PartialMaxInt
{
public:
	static constexpr INT_TYPE PARTIAL_MAX_VALUE = (PartialMaxInt<UNINITIALIZED_SIZE - 1, INT_TYPE>::PARTIAL_MAX_VALUE << 8) | 0xFF;
};
template<typename INT_TYPE>
class PartialMaxInt<1_size, INT_TYPE>
{
public:
	static constexpr INT_TYPE PARTIAL_MAX_VALUE = MaxValueHighestByte<INT_TYPE>();
};
template<typename INT_TYPE>
constexpr INT_TYPE MaxValue() {
	return PartialMaxInt<sizeof(INT_TYPE), INT_TYPE>::PARTIAL_MAX_VALUE;
}
template<typename INT_TYPE>
constexpr INT_TYPE MinValue() {
	return INT_TYPE(MaxValue<INT_TYPE>() + INT_TYPE(1));
}

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