#include "test.h"

using namespace stateos;

static auto Mtx0 = Mutex(mtxDefault);
static auto Mtx1 = Mutex(mtxErrorCheck);
static auto Mtx2 = Mutex(mtxPrioInherit);
static auto Mtx3 = Mutex(mtxPrioProtect, 3);

static auto Cnd0 = ConditionVariable();
static auto Cnd1 = ConditionVariable();
static auto Cnd2 = ConditionVariable();
static auto Cnd3 = ConditionVariable();

static void proc3()
{
	int result;

	result = Mtx3.wait();                         ASSERT_success(result);
	result = Cnd3.wait(Mtx3);                     ASSERT_success(result);
	result = Mtx2.wait();                         ASSERT_success(result);
	         Cnd2.give(cndOne);
	result = Mtx2.give();                         ASSERT_success(result);
	result = Mtx3.give();                         ASSERT_success(result);
	         thisTask::stop();
}

static void proc2()
{
	int result;

	result = Mtx2.wait();                         ASSERT_success(result);
	                                              ASSERT(!Tsk3);
	         Tsk3.startFrom(proc3);               ASSERT(!!Tsk3);
	result = Cnd2.wait(Mtx2);                     ASSERT_success(result);
	result = Mtx3.wait();                         ASSERT_success(result);
	         Cnd3.give(cndOne);
	result = Mtx3.give();                         ASSERT_success(result);
	result = Cnd2.wait(Mtx2);                     ASSERT_success(result);
	result = Mtx1.wait();                         ASSERT_success(result);
	         Cnd1.give(cndOne);
	result = Mtx1.give();                         ASSERT_success(result);
	result = Mtx2.give();                         ASSERT_success(result);
	result = Tsk3.join();                         ASSERT_success(result);
	         thisTask::stop();
}

static void proc1()
{
	int result;

	result = Mtx1.wait();                         ASSERT_success(result);
	                                              ASSERT(!Tsk2);
	         Tsk2.startFrom(proc2);               ASSERT(!!Tsk2);
	result = Cnd1.wait(Mtx1);                     ASSERT_success(result);
	result = Mtx2.wait();                         ASSERT_success(result);
	         Cnd2.give(cndOne);
	result = Mtx2.give();                         ASSERT_success(result);
	result = Cnd1.wait(Mtx1);                     ASSERT_success(result);
	result = Mtx0.wait();                         ASSERT_success(result);
	         Cnd0.give(cndOne);
	result = Mtx0.give();                         ASSERT_success(result);
	result = Mtx1.give();                         ASSERT_success(result);
	result = Tsk2.join();                         ASSERT_success(result);
	         thisTask::stop();
}

static void proc0()
{
	int result;

	result = Mtx0.wait();                         ASSERT_success(result);
	                                              ASSERT(!Tsk1);
	         Tsk1.startFrom(proc1);               ASSERT(!!Tsk1);
	result = Cnd0.wait(Mtx0);                     ASSERT_success(result);
	result = Mtx1.wait();                         ASSERT_success(result);
	         Cnd1.give(cndOne);
	result = Mtx1.give();                         ASSERT_success(result);
	result = Cnd0.wait(Mtx0);                     ASSERT_success(result);
	result = Mtx0.give();                         ASSERT_success(result);
	result = Tsk1.join();                         ASSERT_success(result);
	         thisTask::stop();
}

static void test()
{
	int result;
	                                              ASSERT(!Tsk0);
	         Tsk0.startFrom(proc0);               ASSERT(!!Tsk0);
	         thisTask::yield();
	         thisTask::yield();
	result = Mtx0.wait();                         ASSERT_success(result);
	         Cnd0.give(cndOne);
	result = Mtx0.give();                         ASSERT_success(result);
	result = Tsk0.join();                         ASSERT_success(result);
}

extern "C"
void test_condition_variable_3()
{
	TEST_Notify();
	TEST_Call();
}
