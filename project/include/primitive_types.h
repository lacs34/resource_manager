/*
* primitive_types.h
*
*  Created on: 2016Äê3ÔÂ25ÈÕ
*      Author: ÌÎ
*/

#ifndef PRIMITIVE_TYPES_H_
#define PRIMITIVE_TYPES_H_

#include <cstddef>
#include "common_definition.h"

template<std::size_t TYPE_SIZE, typename CANDIDATE_TYPES_HEADER, typename ...CANDIDATE_TYPES>
class TypePicker;

template<bool FIT, std::size_t TYPE_SIZE, typename CANDIDATE_TYPES_HEADER, typename ...CANDIDATE_TYPES>
class TypePickerFitter {
public:
	typedef typename TypePicker<TYPE_SIZE, CANDIDATE_TYPES...>::PICKED_TYPE PICKED_TYPE;
};

template<std::size_t TYPE_SIZE, typename CANDIDATE_TYPES_HEADER, typename ...CANDIDATE_TYPES>
class TypePickerFitter<true, TYPE_SIZE, CANDIDATE_TYPES_HEADER, CANDIDATE_TYPES...> {
public:
	typedef CANDIDATE_TYPES_HEADER PICKED_TYPE;
};

template<std::size_t TYPE_SIZE, typename CANDIDATE_TYPES_HEADER, typename ...CANDIDATE_TYPES>
class TypePicker {
public:
	typedef typename TypePickerFitter<TYPE_SIZE == sizeof(CANDIDATE_TYPES_HEADER), TYPE_SIZE, CANDIDATE_TYPES_HEADER, CANDIDATE_TYPES...>::PICKED_TYPE PICKED_TYPE;
};

template<std::size_t TYPE_SIZE, typename ...CANDIDATE_TYPES>
using PickType = typename TypePicker<TYPE_SIZE, CANDIDATE_TYPES...>::PICKED_TYPE;

template<bool IS_SIGNED, std::size_t TYPE_SIZE>
class IntPicker {
public:
	typedef PickType<TYPE_SIZE, signed char, short, int, long, long long> PICKED_INT;
};

template<std::size_t TYPE_SIZE>
class IntPicker<false, TYPE_SIZE> {
public:
	typedef PickType<TYPE_SIZE, unsigned char, unsigned short, unsigned int, unsigned long, unsigned long long> PICKED_INT;
};

template<bool IS_SIGNED, std::size_t TYPE_SIZE>
using PickInt = typename IntPicker<IS_SIGNED, TYPE_SIZE>::PICKED_INT;

template<std::size_t TYPE_SIZE>
using PickSigned = PickInt<true, TYPE_SIZE>;
template<std::size_t TYPE_SIZE>
using PickUnsigned = PickInt<false, TYPE_SIZE>;

typedef PickSigned<1_size> Int8;
typedef PickSigned<2_size> Int16;
typedef PickSigned<4_size> Int32;
typedef PickSigned<8_size> Int64;

typedef PickUnsigned<1_size> UInt8;
typedef PickUnsigned<2_size> UInt16;
typedef PickUnsigned<4_size> UInt32;
typedef PickUnsigned<8_size> UInt64;

#endif