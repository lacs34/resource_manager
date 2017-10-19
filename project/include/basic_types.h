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
#include <algorithm>

template<typename INT_TYPE>
constexpr bool IsSigned() {
	return INT_TYPE(INT_TYPE(0) - INT_TYPE(1)) < INT_TYPE(0);
}

template<typename INT_TYPE>
constexpr INT_TYPE MaxValueHighestByte() {
	return IsSigned<INT_TYPE>() ? INT_TYPE(0x7F) : INT_TYPE(0xFF);
}
template<std::size_t UNINITIALIZED_SIZE, typename INT_TYPE>
class PartialMaxInt {
public:
	static constexpr INT_TYPE PARTIAL_MAX_VALUE = (PartialMaxInt<UNINITIALIZED_SIZE - 1, INT_TYPE>::PARTIAL_MAX_VALUE << 8) | 0xFF;
};
template<typename INT_TYPE>
class PartialMaxInt<1_size, INT_TYPE> {
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

constexpr bool LessEqualU64(UInt64 lhs, UInt64 rhs) {
	return lhs <= rhs;
}
constexpr UInt64 NextU64MaxValue(UInt64 currentMaxValue) {
	return currentMaxValue * (currentMaxValue + 2);
}
template<bool CAN_STORAGE, std::size_t BYTES,  UInt64 MAX_VALUE, UInt64 STORAGED_VALUE>
class SmallestUnsignedCanStorage {
public:
	typedef PickUnsigned<BYTES> SMALLEST_UNSIGNED_TYPE;
};
template<std::size_t BYTES, UInt64 MAX_VALUE, UInt64 STORAGED_VALUE>
class SmallestUnsignedCanStorage<false, BYTES, MAX_VALUE, STORAGED_VALUE> {
public:
	typedef typename SmallestUnsignedCanStorage<LessEqualU64(STORAGED_VALUE, NextU64MaxValue(MAX_VALUE)), BYTES * 2, NextU64MaxValue(MAX_VALUE), STORAGED_VALUE>::SMALLEST_UNSIGNED_TYPE SMALLEST_UNSIGNED_TYPE;
};
template<UInt64 STORAGED_VALUE>
using SmallestUnsignedStorage = typename SmallestUnsignedCanStorage<LessEqualU64(STORAGED_VALUE, 0xFF), 1_size, 0xFF, STORAGED_VALUE>::SMALLEST_UNSIGNED_TYPE;

constexpr bool LessEqualI64(Int64 lhs, Int64 rhs) {
	return lhs <= rhs;
}
constexpr Int64 NextI64MaxValue(Int64 currentMaxValue) {
	return currentMaxValue * (currentMaxValue + 2) * 2 + 1;
}
constexpr Int64 NextI64MinValue(Int64 currentMinValue) {
	return currentMinValue * (-currentMinValue) * 2;
}
template<bool CAN_STORAGE, std::size_t BYTES, Int64 MAX_VALUE, Int64 STORAGED_VALUE>
class SmallestSignedCanStorageMax {
public:
	static constexpr std::size_t NEED_BYTES = BYTES;
};
template<std::size_t BYTES, Int64 MAX_VALUE, Int64 STORAGED_VALUE>
class SmallestSignedCanStorageMax<false, BYTES, MAX_VALUE, STORAGED_VALUE> {
public:
	static constexpr std::size_t NEED_BYTES = SmallestSignedCanStorageMax<LessEqualI64(STORAGED_VALUE, NextI64MaxValue(MAX_VALUE)), BYTES * 2, NextI64MaxValue(MAX_VALUE), STORAGED_VALUE>::NEED_BYTES;
};
template<bool CAN_STORAGE, std::size_t BYTES, Int64 MIN_VALUE, Int64 STORAGED_VALUE>
class SmallestSignedCanStorageMin {
public:
	static constexpr std::size_t NEED_BYTES = BYTES;
};
template<std::size_t BYTES, Int64 MIN_VALUE, Int64 STORAGED_VALUE>
class SmallestSignedCanStorageMin<false, BYTES, MIN_VALUE, STORAGED_VALUE> {
public:
	static constexpr std::size_t NEED_BYTES = SmallestSignedCanStorageMin<LessEqualI64(NextI64MinValue(MIN_VALUE), STORAGED_VALUE), BYTES * 2, NextI64MinValue(MIN_VALUE), STORAGED_VALUE>::NEED_BYTES;
};
template<bool LESS_THAN_ZERO, Int64 STORAGED_VALUE>
class SmallestSignedCanStorage {
public:
	static constexpr std::size_t NEED_BYTES = SmallestSignedCanStorageMin<LessEqualI64(-128, STORAGED_VALUE), 1, -128, STORAGED_VALUE>::NEED_BYTES;
};
template<Int64 STORAGED_VALUE>
class SmallestSignedCanStorage<false, STORAGED_VALUE> {
public:
	static constexpr std::size_t NEED_BYTES = SmallestSignedCanStorageMax<LessEqualI64(STORAGED_VALUE, 127), 1, 127, STORAGED_VALUE>::NEED_BYTES;
};
template<Int64 FIRST, Int64 ...VALUES>
class PickMaxSize {
private:
	static constexpr std::size_t FIRST_SIZE = SmallestSignedCanStorage<LessEqualI64(FIRST, 0), FIRST>::NEED_BYTES;
	static constexpr std::size_t TAIL_SIZE = PickMaxSize<VALUES...>::INT_SIZE;
public:
	static constexpr std::size_t INT_SIZE = std::max(FIRST_SIZE, TAIL_SIZE);
};
template<Int64 VALUE>
class PickMaxSize<VALUE> {
public:
	static constexpr std::size_t INT_SIZE = SmallestSignedCanStorage<LessEqualI64(VALUE, 0), VALUE>::NEED_BYTES;
};
template<Int64 ...VALUES>
using SmallestSignedStorage = PickSigned<PickMaxSize<VALUES...>::INT_SIZE>;

using IntOr32 = TypeSelect<Less(sizeof(int), 4_size), Int32, int>::SELECTED_TYPE;

typedef PickUnsigned<sizeof(void*)> UIntPointer;

template<typename POINTER_TYPE>
constexpr UIntPointer ToUInt(POINTER_TYPE *pointer) {
	return (UIntPointer)pointer;
}

#endif /* BASIC_TYPES_H_ */