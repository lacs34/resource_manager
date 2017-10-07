/*
 * test_predefined_pointers.cpp
 *
 *  Created on: 2016Äê3ÔÂ23ÈÕ
 *      Author: Administrator
 */

#include "predefined_smart_pointers.h"
#include "test_delete_traits.h"
#include "gtest.h"

class PredefinedPointersTest :
	public testing::Test {
};

class TestNewObject {
public:
	static int NewCount;
	static void* operator new(size_t n) {
		++NewCount;
		return malloc(n);
	}
};
int TestNewObject::NewCount = 0;

TEST_F(PredefinedPointersTest, TestStdPointerNewObject)
{
	StdPointer<TestNewObject> testPointer;
	ASSERT_EQ(1, TestNewObject::NewCount);
}

class TestDeleteObject {
private:
	int *m_Record;

public:
	TestDeleteObject(int *record) :
		m_Record(record) {
	}
	static void operator delete(void *p) {
		TestDeleteObject *object = reinterpret_cast<TestDeleteObject*>(p);
		if (object->m_Record != nullptr) {
			++(*object->m_Record);
		}
		::operator delete(p);
	}
};

TEST_F(PredefinedPointersTest, TestStdPointerDeleteOperation)
{
	int deleteCount = 0;
	{
		StdPointer<TestDeleteObject> testPointer(&deleteCount);
	}
	ASSERT_EQ(1, deleteCount);
}

class TestAcceptManager {
private:
	MemoryManager *m_Manager;
	int *m_Record;

public:
	TestAcceptManager(MemoryManager *manager, int *record) :
		m_Manager(manager),
		m_Record(record) {
	}
	void TestManager() {
		m_Manager->IncreaseReference();
	}
	void Release() {
		m_Manager->DecreaseReference();
	}
	static void operator delete(void *p) {
		TestAcceptManager *object = reinterpret_cast<TestAcceptManager*>(p);
		if (object->m_Record != nullptr) {
			++(*object->m_Record);
		}
		::operator delete(p);
	}
	TestAcceptManager* Get() {
		return this;
	}
};

TEST_F(PredefinedPointersTest, TestStdPointerSelfManagerParameter)
{
	int deleteCount = 0;
	TestAcceptManager *ptr = nullptr;
	{
		StdPointer<TestAcceptManager> testPointer(Self, &deleteCount);
		testPointer->TestManager();
		ptr = testPointer->Get();
	}
	ASSERT_EQ(0, deleteCount);
	ptr->Release();
	ASSERT_EQ(1, deleteCount);
}
