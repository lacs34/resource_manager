/*
 * test_string.cpp
 *
 *  Created on: 2016��3��25��
 *      Author: ��
 */

#include "string_lib.h"
#include "gtest.h"

class StringTest :
	public testing::Test {
protected:
};

TEST_F(StringTest, TestStringLength)
{
	String str(String::From<Utf8Encoding>(u8"����"));
	//ASSERT_EQ(6_size, str.t);
}
