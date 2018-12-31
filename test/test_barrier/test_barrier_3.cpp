#include "test.h"

auto Bar0 = Barrier(2);
auto Bar1 = Barrier(2);
auto Bar2 = Barrier(2);
auto Bar3 = Barrier(2);

static void proc3()
{
	unsigned event;

	event = Bar3.wait();                         ASSERT_success(event);
	event = Bar2.wait();                         ASSERT_success(event);
	        ThisTask::stop();
}

static void proc2()
{
	unsigned event;
		                                         ASSERT(!Tsk3);
	        Tsk3.startFrom(proc3);               ASSERT(!!Tsk3);
	event = Bar2.wait();                         ASSERT_success(event);
	event = Bar3.wait();                         ASSERT_success(event);
	event = Bar2.wait();                         ASSERT_success(event);
	event = Bar1.wait();                         ASSERT_success(event);
	event = Tsk3.join();                         ASSERT_success(event);
	        ThisTask::stop();
}

static void proc1()
{
	unsigned event;
		                                         ASSERT(!Tsk2);
	        Tsk2.startFrom(proc2);               ASSERT(!!Tsk2);
	event = Bar1.wait();                         ASSERT_success(event);
	event = Bar2.wait();                         ASSERT_success(event);
	event = Bar1.wait();                         ASSERT_success(event);
	event = Bar0.wait();                         ASSERT_success(event);
	event = Tsk2.join();                         ASSERT_success(event);
	        ThisTask::stop();
}

static void proc0()
{
	unsigned event;
		                                         ASSERT(!Tsk1);
	        Tsk1.startFrom(proc1);               ASSERT(!!Tsk1);
	event = Bar0.wait();                         ASSERT_success(event);
	event = Bar1.wait();                         ASSERT_success(event);
	event = Bar0.wait();                         ASSERT_success(event);
	event = Tsk1.join();                         ASSERT_success(event);
	        ThisTask::stop();
}

static void test()
{
	unsigned event;
		                                         ASSERT(!Tsk0);
	        Tsk0.startFrom(proc0);               ASSERT(!!Tsk0);
	event = Bar0.wait();                         ASSERT_success(event);
	event = Tsk0.join();                         ASSERT_success(event);
}

extern "C"
void test_barrier_3()
{
	TEST_Notify();
	TEST_Call();
}
