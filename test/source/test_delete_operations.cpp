/*
 * test_delete_operations.cpp
 *
 *  Created on: 2016Äê3ÔÂ14ÈÕ
 *      Author: Administrator
 */

#include "delete_operations.h"
#include "test_delete_traits.h"
#include "test_delete_operation.h"
#include "common_definition.h"
#include "gtest.h"

class DeleteOperationTest :
	public testing::Test {
protected:
	template<typename CONTAINER_TYPE>
	void TestDeleteOperationDeleteSelf(CONTAINER_TYPE &container) {
		container->DoDelete();
		ASSERT_EQ(1L, container.m_DeleteCount);
	}
	template<typename EXCEPTION_TYPE, typename CONTAINER_TYPE>
	void TestDeleteOperationDeleteSelfOnException(CONTAINER_TYPE &container) {
		ASSERT_THROW(container->DoDelete(), EXCEPTION_TYPE);
		ASSERT_EQ(1L, container.m_DeleteCount);
	}
};

TEST_F(DeleteOperationTest, TestObjectDeleteOperationDeleteObject)
{
	TestDeleteTraitsObjectContainer<int> traits(1);
	ObjectDeleteOperation<int, TestDeleteTraits, DummyTraits> deleteOperation(&traits.m_ObjectToBeDeleted);
	deleteOperation.DoDelete();
	ASSERT_EQ(1L, traits.m_DeleteCount);
}

TEST_F(DeleteOperationTest, TestObjectDeleteOperationDeleteSelf)
{
	TestDeleteTraitsObjectContainer<ObjectDeleteOperation<int, DummyTraits, TestDeleteTraits>> deleteOperation(nullptr);
	TestDeleteOperationDeleteSelf(deleteOperation);
}

TEST_F(DeleteOperationTest, TestObjectDeleteOperationDeleteSelfOnException)
{
	TestDeleteTraitsObjectContainer<ObjectDeleteOperation<int, TestRaiseExceptionDeleteTraits, TestDeleteTraits>> deleteOperation(nullptr);
	TestDeleteOperationDeleteSelfOnException<TestDeleteTraitsRaisedException>(deleteOperation);
}

TEST_F(DeleteOperationTest, TestForwardDeleteOperationForwardOperation)
{
	TestDeleteOperation testDelete;
	ForwardDeleteOperation<DummyTraits> deleteOperation(&testDelete);
	deleteOperation.DoDelete();
	ASSERT_EQ(1L, testDelete.m_DeleteCount);
}

TEST_F(DeleteOperationTest, TestForwardDeleteOperationDeleteSelf)
{
	DummyDeleteOperation dummyDelete;
	TestDeleteTraitsObjectContainer<ForwardDeleteOperation<TestDeleteTraits>> deleteOperation(&dummyDelete);
	TestDeleteOperationDeleteSelf(deleteOperation);
}

TEST_F(DeleteOperationTest, TestForwardDeleteOperationDeleteSelfOnException)
{
	TestRaiseExceptionDeleteOperation exceptionDelete;
	TestDeleteTraitsObjectContainer<ForwardDeleteOperation<TestDeleteTraits>> deleteOperation(&exceptionDelete);
	TestDeleteOperationDeleteSelfOnException<TestDeleteOperationRaisedException>(deleteOperation);
}

TEST_F(DeleteOperationTest, TestActivateDeleteOperationDeleteThrowExceptionWhenActiveTwice)
{
	ActivateDeleteOperation<DummyDeleteOperation, DummyTraits> deleteOperation;
	deleteOperation.Activate();
	ASSERT_THROW(deleteOperation.Activate(), RepeatedActivationException);
}

TEST_F(DeleteOperationTest, TestActivateDeleteOperationDeleteNotForwardingIfNotActived)
{
	int deleteCount = 0;
	ActivateDeleteOperation<TestExternalRecordDeleteOperation, DummyTraits> deleteOperation;
	deleteOperation.DoDelete();
	ASSERT_EQ(0, deleteCount);
}

TEST_F(DeleteOperationTest, TestActivateDeleteOperationDeleteDoDeleteForwarding)
{
	int deleteCount = 0;
	ActivateDeleteOperation<TestExternalRecordDeleteOperation, DummyTraits> deleteOperation;
	deleteOperation.Activate(&deleteCount);
	deleteOperation.DoDelete();
	ASSERT_EQ(1, deleteCount);
}

TEST_F(DeleteOperationTest, TestActivateDeleteOperationDeleteInternalOperationWhenDestructed)
{
	int destructCount = 0;
	{
		ActivateDeleteOperation<TestDestructionDeleteOperation, DummyTraits> deleteOperation;
		deleteOperation.Activate(&destructCount);
	}
	ASSERT_EQ(1, destructCount);
}

TEST_F(DeleteOperationTest, TesActivateDeleteOperationDeleteSelf)
{
	TestDeleteTraitsObjectContainer<ActivateDeleteOperation<DummyDeleteOperation, TestDeleteTraits>> deleteOperation;
	deleteOperation->Activate();
	TestDeleteOperationDeleteSelf(deleteOperation);
}

TEST_F(DeleteOperationTest, TestActivateDeleteOperationDeleteSelfOnException)
{
	TestDeleteTraitsObjectContainer<ActivateDeleteOperation<TestRaiseExceptionDeleteOperation, TestDeleteTraits>> deleteOperation;
	deleteOperation->Activate();
	TestDeleteOperationDeleteSelfOnException<TestDeleteOperationRaisedException>(deleteOperation);
}
