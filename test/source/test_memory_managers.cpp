/*
 * test_memory_managers.cpp
 *
 *  Created on: 2016Äê3ÔÂ20ÈÕ
 *      Author: Administrator
 */

#include "common_definition.h"
#include "memory_managers.h"
#include "delete_operations.h"
#include "exceptions.h"
#include "test_delete_operation.h"
#include "test_delete_traits.h"
#include "multithread_test.h"
#include "gtest.h"

class MemoryManagerTest :
	public testing::Test {
};

TEST_F(MemoryManagerTest, TestSingleThreadReferenceCountMemoryManagerInitialReferenceCount)
{
	TestDeleteOperation deleteOperation;
	SingleThreadReferenceCountMemoryManager<DummyTraits> memoryManager(&deleteOperation);
	memoryManager.DecreaseReference();
	ASSERT_EQ(1L, deleteOperation.m_DeleteCount);
}

TEST_F(MemoryManagerTest, TestSingleThreadReferenceCountMemoryManagerIncreaseReference)
{
	TestDeleteOperation deleteOperation;
	SingleThreadReferenceCountMemoryManager<DummyTraits> memoryManager(&deleteOperation);
	memoryManager.IncreaseReference();
	memoryManager.DecreaseReference();
	ASSERT_EQ(0L, deleteOperation.m_DeleteCount);
}

TEST_F(MemoryManagerTest, TestSingleThreadReferenceCountMemoryManagerDeleteSelf)
{
	DummyDeleteOperation deleteOperation;
	TestDeleteTraitsObjectContainer<SingleThreadReferenceCountMemoryManager<TestDeleteTraits>> memoryManagerContainer(&deleteOperation);
	memoryManagerContainer->DecreaseReference();
	ASSERT_EQ(1L, memoryManagerContainer.m_DeleteCount);
}

TEST_F(MemoryManagerTest, TestMultiThreadReferenceCountMemoryManagerInitialReferenceCount)
{
	TestDeleteOperation deleteOperation;
	MultiThreadReferenceCountMemoryManager<DummyTraits> memoryManager(&deleteOperation);
	MultiThreadController<4U> controller;
	ThreadKernel *kernel = CreateTestKernel([&]() {
		for (size_t i = 0; i < 4000; ++i) {
			memoryManager.IncreaseReference();
			memoryManager.DecreaseReference();
		} });
	controller.Run(kernel);
	controller.WaitToEnd();
	ASSERT_EQ(0L, deleteOperation.m_DeleteCount);
	delete kernel;
	memoryManager.DecreaseReference();
	ASSERT_EQ(1L, deleteOperation.m_DeleteCount);
}
