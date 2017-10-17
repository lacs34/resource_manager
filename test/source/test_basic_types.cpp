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