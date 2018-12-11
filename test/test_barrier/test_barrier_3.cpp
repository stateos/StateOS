#include "test.h"

auto Bar0 = Barrier(2);
auto Bar1 = Barrier(3);
auto Bar2 = Barrier(4);
auto Bar3 = Barrier(5);

static void proc3()
{
	unsigned event;

	event = Bar3.wait();                         assert_success(event);
	        ThisTask::stop();
}

static void proc2()
{
	unsigned event;
		                                         assert_dead(tsk3);
	        tsk_startFrom(tsk3, proc3);          assert_ready(tsk3);
	event = Bar2.wait();                         assert_success(event);
	event = Bar3.wait();                         assert_success(event);
	event = tsk_join(tsk3);                      assert_success(event);
	        ThisTask::stop();
}

static void proc1()
{
	unsigned event;
		                                         assert(!Tsk2);
	        Tsk2.startFrom(proc2);               assert(!!Tsk2);
	event = Bar1.wait();                         assert_success(event);
	event = Bar2.wait();                         assert_success(event);
	event = Bar3.wait();                         assert_success(event);
	event = Tsk2.join();                         assert_success(event);
	        ThisTask::stop();
}

static void proc0()
{
	unsigned event;
		                                         assert(!Tsk1);
	        Tsk1.startFrom(proc1);               assert(!!Tsk1);
	event = Bar0.wait();                         assert_success(event);
	event = Bar1.wait();                         assert_success(event);
	event = Bar2.wait();                         assert_success(event);
	event = Bar3.wait();                         assert_success(event);
	event = Tsk1.join();                         assert_success(event);
	        ThisTask::stop();
}

static void test()
{
	unsigned event;
		                                         assert_dead(&tsk0);
	        tsk_startFrom(&tsk0, proc0);         assert_ready(&tsk0);
	event = Bar0.wait();                         assert_success(event);
	event = Bar1.wait();                         assert_success(event);
	event = Bar2.wait();                         assert_success(event);
	event = Bar3.wait();                         assert_success(event);
	event = tsk_join(&tsk0);                     assert_success(event);
}

extern "C"
void test_barrier_3()
{
	TEST_Notify();
	TEST_Call();
}
