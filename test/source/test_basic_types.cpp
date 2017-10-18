/*
* test_basic_types.cpp
*
*  Created on: 2016Äê3ÔÂ25ÈÕ
*      Author: ÌÎ
*/

#include "gtest.h"
#include "basic_types.h"

class BasicTypesTest :
	public testing::Test {
protected:
};

TEST_F(BasicTypesTest, TestTypeSize)
{
	ASSERT_EQ(1_size, sizeof(Int8));
	ASSERT_EQ(2_size, sizeof(Int16));
	ASSERT_EQ(4_size, sizeof(Int32));
	ASSERT_EQ(8_size, sizeof(Int64));

	ASSERT_EQ(1_size, sizeof(UInt8));
	ASSERT_EQ(2_size, sizeof(UInt16));
	ASSERT_EQ(4_size, sizeof(UInt32));
	ASSERT_EQ(8_size, sizeof(UInt64));
}

TEST_F(BasicTypesTest, TestIsSigned)
{
	ASSERT_EQ(true, IsSigned<signed char>());
	ASSERT_EQ(true, IsSigned<short>());
	ASSERT_EQ(true, IsSigned<int>());
	ASSERT_EQ(true, IsSigned<long>());
	ASSERT_EQ(true, IsSigned<long long>());

	ASSERT_EQ(false, IsSigned<unsigned char>());
	ASSERT_EQ(false, IsSigned<unsigned short>());
	ASSERT_EQ(false, IsSigned<unsigned int>());
	ASSERT_EQ(false, IsSigned<unsigned long>());
	ASSERT_EQ(false, IsSigned<unsigned long long>());
}

TEST_F(BasicTypesTest, TestTypeSigned)
{
	ASSERT_EQ(true, IsSigned<Int8>());
	ASSERT_EQ(true, IsSigned<Int16>());
	ASSERT_EQ(true, IsSigned<Int32>());
	ASSERT_EQ(true, IsSigned<Int64>());

	ASSERT_EQ(false, IsSigned<UInt8>());
	ASSERT_EQ(false, IsSigned<UInt16>());
	ASSERT_EQ(false, IsSigned<UInt32>());
	ASSERT_EQ(false, IsSigned<UInt64>());
}

TEST_F(BasicTypesTest, TestMaxValue)
{
	ASSERT_EQ(0xFF, MaxValue<UInt8>());
	ASSERT_EQ(0xFFFF, MaxValue<UInt16>());
	ASSERT_EQ(0xFFFFFFFF, MaxValue<UInt32>());
	ASSERT_EQ(0xFFFFFFFFFFFFFFFF, MaxValue<UInt64>());

	ASSERT_EQ(0x7F, MaxValue<Int8>());
	ASSERT_EQ(0x7FFF, MaxValue<Int16>());
	ASSERT_EQ(0x7FFFFFFF, MaxValue<Int32>());
	ASSERT_EQ(0x7FFFFFFFFFFFFFFF, MaxValue<Int64>());
}

TEST_F(BasicTypesTest, TestMinValue)
{
	ASSERT_EQ(0, MinValue<UInt8>());
	ASSERT_EQ(0, MinValue<UInt16>());
	ASSERT_EQ(0, MinValue<UInt32>());
	ASSERT_EQ(0, MinValue<UInt64>());

	ASSERT_EQ(Int8(0x80), MinValue<Int8>());
	ASSERT_EQ(Int16(0x8000), MinValue<Int16>());
	ASSERT_EQ(Int32(0x80000000), MinValue<Int32>());
	ASSERT_EQ(Int64(0x8000000000000000), MinValue<Int64>());
}

TEST_F(BasicTypesTest, TestSmallestUnsignedStorage)
{
	ASSERT_EQ(1, sizeof(SmallestUnsignedStorage<0>));
	ASSERT_EQ(1, sizeof(SmallestUnsignedStorage<0xFF>));

	ASSERT_EQ(2, sizeof(SmallestUnsignedStorage<0x100>));
	ASSERT_EQ(2, sizeof(SmallestUnsignedStorage<0xFFFF>));

	ASSERT_EQ(4, sizeof(SmallestUnsignedStorage<0x10000>));
	ASSERT_EQ(4, sizeof(SmallestUnsignedStorage<0xFFFFFFFF>));

	ASSERT_EQ(8, sizeof(SmallestUnsignedStorage<0x100000000>));
	ASSERT_EQ(8, sizeof(SmallestUnsignedStorage<0xFFFFFFFFFFFFFFFF>));
}

TEST_F(BasicTypesTest, TestSmallestSignedStorage)
{
	ASSERT_EQ(1, sizeof(SmallestSignedStorage<0, 0>));
	ASSERT_EQ(1, sizeof(SmallestSignedStorage<0, 0x7F>));

	ASSERT_EQ(2, sizeof(SmallestSignedStorage<0, 0x80>));
	ASSERT_EQ(2, sizeof(SmallestSignedStorage<0, 0x7FFF>));

	ASSERT_EQ(4, sizeof(SmallestSignedStorage<0, 0x8000>));
	ASSERT_EQ(4, sizeof(SmallestSignedStorage<0, 0x7FFFFFFF>));

	ASSERT_EQ(8, sizeof(SmallestSignedStorage<0, 0x80000000>));
	ASSERT_EQ(8, sizeof(SmallestSignedStorage<0, 0x7FFFFFFFFFFFFFFF>));


	ASSERT_EQ(1, sizeof(SmallestSignedStorage<-Int64(128), 0>));
	ASSERT_EQ(1, sizeof(SmallestSignedStorage<-Int64(128), 0x7F>));

	ASSERT_EQ(2, sizeof(SmallestSignedStorage<-Int64(128), 0x80>));
	ASSERT_EQ(2, sizeof(SmallestSignedStorage<-Int64(128), 0x7FFF>));

	ASSERT_EQ(4, sizeof(SmallestSignedStorage<-Int64(128), 0x8000>));
	ASSERT_EQ(4, sizeof(SmallestSignedStorage<-Int64(128), 0x7FFFFFFF>));

	ASSERT_EQ(8, sizeof(SmallestSignedStorage<-Int64(128), 0x80000000>));
	ASSERT_EQ(8, sizeof(SmallestSignedStorage<-Int64(128), 0x7FFFFFFFFFFFFFFF>));


	ASSERT_EQ(2, sizeof(SmallestSignedStorage<-Int64(129), 0>));
	ASSERT_EQ(2, sizeof(SmallestSignedStorage<-Int64(129), 0x7F>));

	ASSERT_EQ(2, sizeof(SmallestSignedStorage<-Int64(129), 0x80>));
	ASSERT_EQ(2, sizeof(SmallestSignedStorage<-Int64(129), 0x7FFF>));

	ASSERT_EQ(4, sizeof(SmallestSignedStorage<-Int64(129), 0x8000>));
	ASSERT_EQ(4, sizeof(SmallestSignedStorage<-Int64(129), 0x7FFFFFFF>));

	ASSERT_EQ(8, sizeof(SmallestSignedStorage<-Int64(129), 0x80000000>));
	ASSERT_EQ(8, sizeof(SmallestSignedStorage<-Int64(129), 0x7FFFFFFFFFFFFFFF>));


	ASSERT_EQ(2, sizeof(SmallestSignedStorage<-Int64(32768), 0>));
	ASSERT_EQ(2, sizeof(SmallestSignedStorage<-Int64(32768), 0x7F>));

	ASSERT_EQ(2, sizeof(SmallestSignedStorage<-Int64(32768), 0x80>));
	ASSERT_EQ(2, sizeof(SmallestSignedStorage<-Int64(32768), 0x7FFF>));

	ASSERT_EQ(4, sizeof(SmallestSignedStorage<-Int64(32768), 0x8000>));
	ASSERT_EQ(4, sizeof(SmallestSignedStorage<-Int64(32768), 0x7FFFFFFF>));

	ASSERT_EQ(8, sizeof(SmallestSignedStorage<-Int64(32768), 0x80000000>));
	ASSERT_EQ(8, sizeof(SmallestSignedStorage<-Int64(32768), 0x7FFFFFFFFFFFFFFF>));


	ASSERT_EQ(4, sizeof(SmallestSignedStorage<-Int64(32769), 0>));
	ASSERT_EQ(4, sizeof(SmallestSignedStorage<-Int64(32769), 0x7F>));

	ASSERT_EQ(4, sizeof(SmallestSignedStorage<-Int64(32769), 0x80>));
	ASSERT_EQ(4, sizeof(SmallestSignedStorage<-Int64(32769), 0x7FFF>));

	ASSERT_EQ(4, sizeof(SmallestSignedStorage<-Int64(32769), 0x8000>));
	ASSERT_EQ(4, sizeof(SmallestSignedStorage<-Int64(32769), 0x7FFFFFFF>));

	ASSERT_EQ(8, sizeof(SmallestSignedStorage<-Int64(32769), 0x80000000>));
	ASSERT_EQ(8, sizeof(SmallestSignedStorage<-Int64(32769), 0x7FFFFFFFFFFFFFFF>));


	ASSERT_EQ(4, sizeof(SmallestSignedStorage<-Int64(2147483648), 0>));
	ASSERT_EQ(4, sizeof(SmallestSignedStorage<-Int64(2147483648), 0x7F>));

	ASSERT_EQ(4, sizeof(SmallestSignedStorage<-Int64(2147483648), 0x80>));
	ASSERT_EQ(4, sizeof(SmallestSignedStorage<-Int64(2147483648), 0x7FFF>));

	ASSERT_EQ(4, sizeof(SmallestSignedStorage<-Int64(2147483648), 0x8000>));
	ASSERT_EQ(4, sizeof(SmallestSignedStorage<-Int64(2147483648), 0x7FFFFFFF>));

	ASSERT_EQ(8, sizeof(SmallestSignedStorage<-Int64(2147483648), 0x80000000>));
	ASSERT_EQ(8, sizeof(SmallestSignedStorage<-Int64(2147483648), 0x7FFFFFFFFFFFFFFF>));


	ASSERT_EQ(8, sizeof(SmallestSignedStorage<-Int64(2147483649), 0>));
	ASSERT_EQ(8, sizeof(SmallestSignedStorage<-Int64(2147483649), 0x7F>));

	ASSERT_EQ(8, sizeof(SmallestSignedStorage<-Int64(2147483649), 0x80>));
	ASSERT_EQ(8, sizeof(SmallestSignedStorage<-Int64(2147483649), 0x7FFF>));

	ASSERT_EQ(8, sizeof(SmallestSignedStorage<-Int64(2147483649), 0x8000>));
	ASSERT_EQ(8, sizeof(SmallestSignedStorage<-Int64(2147483649), 0x7FFFFFFF>));

	ASSERT_EQ(8, sizeof(SmallestSignedStorage<-Int64(2147483649), 0x80000000>));
	ASSERT_EQ(8, sizeof(SmallestSignedStorage<-Int64(2147483649), 0x7FFFFFFFFFFFFFFF>));


	ASSERT_EQ(8, sizeof(SmallestSignedStorage<-Int64(9223372036854775808), 0>));
	ASSERT_EQ(8, sizeof(SmallestSignedStorage<-Int64(9223372036854775808), 0x7F>));

	ASSERT_EQ(8, sizeof(SmallestSignedStorage<-Int64(9223372036854775808), 0x80>));
	ASSERT_EQ(8, sizeof(SmallestSignedStorage<-Int64(9223372036854775808), 0x7FFF>));

	ASSERT_EQ(8, sizeof(SmallestSignedStorage<-Int64(9223372036854775808), 0x8000>));
	ASSERT_EQ(8, sizeof(SmallestSignedStorage<-Int64(9223372036854775808), 0x7FFFFFFF>));

	ASSERT_EQ(8, sizeof(SmallestSignedStorage<-Int64(9223372036854775808), 0x80000000>));
	ASSERT_EQ(8, sizeof(SmallestSignedStorage<-Int64(9223372036854775808), 0x7FFFFFFFFFFFFFFF>));
}