#include "test.h"

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
	unsigned event;

	event = Mtx3.wait();                         ASSERT_success(event);
	event = Cnd3.wait(Mtx3);                     ASSERT_success(event);
	event = Mtx2.wait();                         ASSERT_success(event);
	        Cnd2.give(cndOne);
	event = Mtx2.give();                         ASSERT_success(event);
	event = Mtx3.give();                         ASSERT_success(event);
	        ThisTask::stop();
}

static void proc2()
{
	unsigned event;

	event = Mtx2.wait();                         ASSERT_success(event);
	                                             ASSERT(!Tsk3);
	        Tsk3.startFrom(proc3);               ASSERT(!!Tsk3);
	event = Cnd2.wait(Mtx2);                     ASSERT_success(event);
	event = Mtx3.wait();                         ASSERT_success(event);
	        Cnd3.give(cndOne);
	event = Mtx3.give();                         ASSERT_success(event);
	event = Cnd2.wait(Mtx2);                     ASSERT_success(event);
	event = Mtx1.wait();                         ASSERT_success(event);
	        Cnd1.give(cndOne);
	event = Mtx1.give();                         ASSERT_success(event);
	event = Mtx2.give();                         ASSERT_success(event);
	event = Tsk3.join();                         ASSERT_success(event);
	        ThisTask::stop();
}

static void proc1()
{
	unsigned event;

	event = Mtx1.wait();                         ASSERT_success(event);
	                                             ASSERT(!Tsk2);
	        Tsk2.startFrom(proc2);               ASSERT(!!Tsk2);
	event = Cnd1.wait(Mtx1);                     ASSERT_success(event);
	event = Mtx2.wait();                         ASSERT_success(event);
	        Cnd2.give(cndOne);
	event = Mtx2.give();                         ASSERT_success(event);
	event = Cnd1.wait(Mtx1);                     ASSERT_success(event);
	event = Mtx0.wait();                         ASSERT_success(event);
	        Cnd0.give(cndOne);
	event = Mtx0.give();                         ASSERT_success(event);
	event = Mtx1.give();                         ASSERT_success(event);
	event = Tsk2.join();                         ASSERT_success(event);
	        ThisTask::stop();
}

static void proc0()
{
	unsigned event;

	event = Mtx0.wait();                         ASSERT_success(event);
	                                             ASSERT(!Tsk1);
	        Tsk1.startFrom(proc1);               ASSERT(!!Tsk1);
	event = Cnd0.wait(Mtx0);                     ASSERT_success(event);
	event = Mtx1.wait();                         ASSERT_success(event);
	        Cnd1.give(cndOne);
	event = Mtx1.give();                         ASSERT_success(event);
	event = Cnd0.wait(Mtx0);                     ASSERT_success(event);
	event = Mtx0.give();                         ASSERT_success(event);
	event = Tsk1.join();                         ASSERT_success(event);
	        ThisTask::stop();
}

static void test()
{
	unsigned event;
	                                             ASSERT(!Tsk0);
	        Tsk0.startFrom(proc0);               ASSERT(!!Tsk0);
	        ThisTask::yield();
	        ThisTask::yield();
	event = Mtx0.wait();                         ASSERT_success(event);
	        Cnd0.give(cndOne);
	event = Mtx0.give();                         ASSERT_success(event);
	event = Tsk0.join();                         ASSERT_success(event);
}

extern "C"
void test_condition_variable_3()
{
	TEST_Notify();
	TEST_Call();
}
