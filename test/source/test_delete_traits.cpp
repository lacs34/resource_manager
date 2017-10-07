/*
 * test_delete_traits.cpp
 *
 *  Created on: 2016Äê3ÔÂ19ÈÕ
 *      Author: Administrator
 */

#include "delete_traits.h"
#include "test_destructor_invoked_object.h"
#include "test_delete_count.h"
#include "gtest.h"

class DeleteHook {
public:
	virtual void NotifyDelete(void *address) = 0;
	virtual ~DeleteHook() { }
};

DeleteHook *g_Hooks = nullptr;
void operator delete(void *address) {

}

class DeleteTraitsTest :
	public testing::Test {
};

TEST_F(DeleteTraitsTest, TestStdDeleteTraitsInvokeObjectDestructor)
{
	int destructCount = 0;
	TestDestructorInvokedObject *testObject = new TestDestructorInvokedObject(&destructCount);
	StdDeleteTraits::DeleteObject(testObject);
	ASSERT_EQ(1, destructCount);
}

// TODO: still need to test global new operator

TEST_F(DeleteTraitsTest, TestStdDeleteTraitsInvokeClassDefinedDeleteOperator)
{
	int deleteCount = 0;
	TestDeleteCountObject *testObject = new TestDeleteCountObject(&deleteCount);
	StdDeleteTraits::DeleteObject(testObject);
	ASSERT_EQ(1, deleteCount);
}

TEST_F(DeleteTraitsTest, TestDestructTraitsInvokeObjectDestructor)
{
	int destructCount = 0;
	TestDestructorInvokedObject *testObject = new TestDestructorInvokedObject(&destructCount);
	DestructTraits::DeleteObject(testObject);
	ASSERT_EQ(1, destructCount);
}

TEST_F(DeleteTraitsTest, TestObjectFreeTraitsInvokeObjectDestructor)
{
	int destructCount = 0;
	TestDestructorInvokedObject *testObject = new TestDestructorInvokedObject(&destructCount);
	ObjectFreeTraits::DeleteObject(testObject);
	ASSERT_EQ(1, destructCount);
}
