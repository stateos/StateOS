#include "test.h"

using namespace stateos;

auto Mtx0 = Mutex(mtxNormal);
auto Mtx1 = Mutex(mtxErrorCheck);
auto Mtx2 = Mutex(mtxRecursive);
auto Mtx3 = Mutex(mtxRobust);

static void proc5()
{
	int result;

	result = Mtx3.wait();                         ASSERT_owndead(result);
	result = Mtx3.give();                         ASSERT_success(result);
	         this_task::stop();
}

static void proc4()
{
	int result;

	result = Mtx3.wait();                         ASSERT_success(result);
	                                              ASSERT(!Tsk5);
	         Tsk5.startFrom(proc5);               ASSERT(!!Tsk5);
	result = Mtx1.wait();                         ASSERT(!"test program cannot be caught here");
}

static void proc3()
{
	int result;

	result = Mtx2.take();                         ASSERT_timeout(result);
	result = Mtx3.wait();                         ASSERT_success(result);
	result = Mtx3.take();                         ASSERT_timeout(result);
	                                              ASSERT(!Tsk4);
	         Tsk4.startFrom(proc4);               ASSERT(!!Tsk4);
	result = Mtx3.give();                         ASSERT_success(result);
	result = Mtx3.give();                         ASSERT_failure(result);
	result = Tsk4.kill();                         ASSERT_success(result);
	result = Tsk5.join();                         ASSERT_success(result);
	         this_task::stop();
}

static void proc2()
{
	int result;

	result = Mtx1.take();                         ASSERT_timeout(result);
	result = Mtx2.wait();                         ASSERT_success(result);
	result = Mtx2.take();                         ASSERT_success(result);
	                                              ASSERT(!Tsk3);
	         Tsk3.startFrom(proc3);               ASSERT(!Tsk3);
	result = Tsk3.join();                         ASSERT_success(result);
	result = Mtx2.give();                         ASSERT_success(result);
	result = Mtx2.give();                         ASSERT_success(result);
	         this_task::stop();
}

static void proc1()
{
	int result;

	result = Mtx0.take();                         ASSERT_timeout(result);
	result = Mtx1.wait();                         ASSERT_success(result);
	result = Mtx1.take();                         ASSERT_failure(result);
	                                              ASSERT(!Tsk2);
	         Tsk2.startFrom(proc2);               ASSERT(!Tsk2);
	result = Tsk2.join();                         ASSERT_success(result);
	result = Mtx1.give();                         ASSERT_success(result);
	result = Mtx1.give();                         ASSERT_failure(result);
	         this_task::stop();
}

static void proc0()
{
	int result;

	result = Mtx0.wait();                         ASSERT_success(result);
	result = Mtx0.take();                         ASSERT_timeout(result);
	                                              ASSERT(!Tsk1);
	         Tsk1.startFrom(proc1);               ASSERT(!Tsk1);
	result = Tsk1.join();                         ASSERT_success(result);
	result = Mtx0.give();                         ASSERT_success(result);
	result = Mtx0.give();                         ASSERT_success(result);
	         this_task::stop();
}

static void test()
{
	int result;
	                                              ASSERT(!Tsk0);
	         Tsk0.startFrom(proc0);
	result = Tsk0.join();                         ASSERT_success(result);
}

extern "C"
void test_mutex_5()
{
	TEST_Notify();
	TEST_Call();
}
