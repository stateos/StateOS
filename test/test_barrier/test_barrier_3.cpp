#include "test.h"

using namespace stateos;

auto Bar0 = Barrier(2);
auto Bar1 = Barrier(2);
auto Bar2 = Barrier(2);
auto Bar3 = Barrier(2);

static void proc3()
{
	int result;

	result = Bar3.wait();                         ASSERT_success(result);
	result = Bar2.wait();                         ASSERT_success(result);
	         this_task::stop();
}

static void proc2()
{
	int result;
	                                              ASSERT(!Tsk3);
	         Tsk3.startFrom(proc3);               ASSERT(!!Tsk3);
	result = Bar2.wait();                         ASSERT_success(result);
	result = Bar3.wait();                         ASSERT_success(result);
	result = Bar2.wait();                         ASSERT_success(result);
	result = Bar1.wait();                         ASSERT_success(result);
	result = Tsk3.join();                         ASSERT_success(result);
	         this_task::stop();
}

static void proc1()
{
	int result;
	                                              ASSERT(!Tsk2);
	         Tsk2.startFrom(proc2);               ASSERT(!!Tsk2);
	result = Bar1.wait();                         ASSERT_success(result);
	result = Bar2.wait();                         ASSERT_success(result);
	result = Bar1.wait();                         ASSERT_success(result);
	result = Bar0.wait();                         ASSERT_success(result);
	result = Tsk2.join();                         ASSERT_success(result);
	         this_task::stop();
}

static void proc0()
{
	int result;
	                                              ASSERT(!Tsk1);
	         Tsk1.startFrom(proc1);               ASSERT(!!Tsk1);
	result = Bar0.wait();                         ASSERT_success(result);
	result = Bar1.wait();                         ASSERT_success(result);
	result = Bar0.wait();                         ASSERT_success(result);
	result = Tsk1.join();                         ASSERT_success(result);
	         this_task::stop();
}

static void test()
{
	int result;
	                                              ASSERT(!Tsk0);
	         Tsk0.startFrom(proc0);               ASSERT(!!Tsk0);
	result = Bar0.wait();                         ASSERT_success(result);
	result = Tsk0.join();                         ASSERT_success(result);
}

extern "C"
void test_barrier_3()
{
	TEST_Notify();
	TEST_Call();
}
