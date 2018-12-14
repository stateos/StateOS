#include "test.h"

auto Mtx0 = Mutex(mtxNormal);
auto Mtx1 = Mutex(mtxErrorCheck);
auto Mtx2 = Mutex(mtxRecursive);
auto Mtx3 = Mutex(mtxRobust);

static void proc5()
{
	unsigned event;

	event = Mtx3.wait();                         assert_owndead(event);
	event = Mtx3.give();                         assert_success(event);
	        ThisTask::stop();
}

static void proc4()
{
	unsigned event;

	event = Mtx3.wait();                         assert_success(event);
	                                             assert(!Tsk5);
	        Tsk5.startFrom(proc5);               assert(!!Tsk5);
	event = Mtx1.wait();                         assert(!"test program cannot be caught here");
}

static void proc3()
{
	unsigned event;

	event = Mtx2.take();                         assert_timeout(event);
	event = Mtx3.wait();                         assert_success(event);
	event = Mtx3.take();                         assert_timeout(event);
	                                             assert(!Tsk4);
	        Tsk4.startFrom(proc4);               assert(!!Tsk4);
	event = Mtx3.give();                         assert_success(event);
	event = Mtx3.give();                         assert_failure(event);
	event = Tsk4.kill();                         assert_success(event);
	event = Tsk5.join();                         assert_success(event);
	        ThisTask::stop();
}

static void proc2()
{
	unsigned event;

	event = Mtx1.take();                         assert_timeout(event);
	event = Mtx2.wait();                         assert_success(event);
	event = Mtx2.take();                         assert_success(event);
	                                             assert(!Tsk3);
	        Tsk3.startFrom(proc3);               assert(!Tsk3);
	event = Tsk3.join();                         assert_success(event);
	event = Mtx2.give();                         assert_success(event);
	event = Mtx2.give();                         assert_success(event);
	        ThisTask::stop();
}

static void proc1()
{
	unsigned event;

	event = Mtx0.take();                         assert_timeout(event);
	event = Mtx1.wait();                         assert_success(event);
	event = Mtx1.take();                         assert_failure(event);
	                                             assert(!Tsk2);
	        Tsk2.startFrom(proc2);               assert(!Tsk2);
	event = Tsk2.join();                         assert_success(event);
	event = Mtx1.give();                         assert_success(event);
	event = Mtx1.give();                         assert_failure(event);
	        ThisTask::stop();
}

static void proc0()
{
	unsigned event;

	event = Mtx0.wait();                         assert_success(event);
	event = Mtx0.take();                         assert_timeout(event);
	                                             assert(!Tsk1);
	        Tsk1.startFrom(proc1);               assert(!Tsk1);
	event = Tsk1.join();                         assert_success(event);
	event = Mtx0.give();                         assert_success(event);
	event = Mtx0.give();                         assert_success(event);
	        ThisTask::stop();
}

static void test()
{
	unsigned event;
	                                             assert(!Tsk0);
	        Tsk0.startFrom(proc0);
	event = Tsk0.join();                         assert_success(event);
}

extern "C"
void test_mutex_5()
{
	TEST_Notify();
	TEST_Call();
}
