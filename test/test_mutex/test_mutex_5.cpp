#include "test.h"

auto Mtx0 = Mutex(mtxNormal);
auto Mtx1 = Mutex(mtxErrorCheck);
auto Mtx2 = Mutex(mtxRecursive);
auto Mtx3 = Mutex(mtxRobust);

static void proc5()
{
	unsigned event;

	event = Mtx3.wait();                         ASSERT_owndead(event);
	event = Mtx3.give();                         ASSERT_success(event);
	        ThisTask::stop();
}

static void proc4()
{
	unsigned event;

	event = Mtx3.wait();                         ASSERT_success(event);
	                                             ASSERT(!Tsk5);
	        Tsk5.startFrom(proc5);               ASSERT(!!Tsk5);
	event = Mtx1.wait();                         ASSERT(!"test program cannot be caught here");
}

static void proc3()
{
	unsigned event;

	event = Mtx2.take();                         ASSERT_timeout(event);
	event = Mtx3.wait();                         ASSERT_success(event);
	event = Mtx3.take();                         ASSERT_timeout(event);
	                                             ASSERT(!Tsk4);
	        Tsk4.startFrom(proc4);               ASSERT(!!Tsk4);
	event = Mtx3.give();                         ASSERT_success(event);
	event = Mtx3.give();                         ASSERT_failure(event);
	event = Tsk4.kill();                         ASSERT_success(event);
	event = Tsk5.join();                         ASSERT_success(event);
	        ThisTask::stop();
}

static void proc2()
{
	unsigned event;

	event = Mtx1.take();                         ASSERT_timeout(event);
	event = Mtx2.wait();                         ASSERT_success(event);
	event = Mtx2.take();                         ASSERT_success(event);
	                                             ASSERT(!Tsk3);
	        Tsk3.startFrom(proc3);               ASSERT(!Tsk3);
	event = Tsk3.join();                         ASSERT_success(event);
	event = Mtx2.give();                         ASSERT_success(event);
	event = Mtx2.give();                         ASSERT_success(event);
	        ThisTask::stop();
}

static void proc1()
{
	unsigned event;

	event = Mtx0.take();                         ASSERT_timeout(event);
	event = Mtx1.wait();                         ASSERT_success(event);
	event = Mtx1.take();                         ASSERT_failure(event);
	                                             ASSERT(!Tsk2);
	        Tsk2.startFrom(proc2);               ASSERT(!Tsk2);
	event = Tsk2.join();                         ASSERT_success(event);
	event = Mtx1.give();                         ASSERT_success(event);
	event = Mtx1.give();                         ASSERT_failure(event);
	        ThisTask::stop();
}

static void proc0()
{
	unsigned event;

	event = Mtx0.wait();                         ASSERT_success(event);
	event = Mtx0.take();                         ASSERT_timeout(event);
	                                             ASSERT(!Tsk1);
	        Tsk1.startFrom(proc1);               ASSERT(!Tsk1);
	event = Tsk1.join();                         ASSERT_success(event);
	event = Mtx0.give();                         ASSERT_success(event);
	event = Mtx0.give();                         ASSERT_success(event);
	        ThisTask::stop();
}

static void test()
{
	unsigned event;
	                                             ASSERT(!Tsk0);
	        Tsk0.startFrom(proc0);
	event = Tsk0.join();                         ASSERT_success(event);
}

extern "C"
void test_mutex_5()
{
	TEST_Notify();
	TEST_Call();
}
