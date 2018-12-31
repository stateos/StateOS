#include "test.h"

static auto Mut0 = FastMutex();
static auto Mut1 = FastMutex();
static auto Mut2 = FastMutex();
static auto Mut3 = FastMutex();

static void proc3()
{
	unsigned event;

	event = Mut2.take();                         ASSERT_timeout(event);
	event = Mut3.wait();                         ASSERT_success(event);
	event = Mut3.take();                         ASSERT_failure(event);
	event = Mut3.give();                         ASSERT_success(event);
	event = Mut3.give();                         ASSERT_failure(event);
	        ThisTask::stop();
}

static void proc2()
{
	unsigned event;

	event = Mut1.take();                         ASSERT_timeout(event);
	event = Mut2.wait();                         ASSERT_success(event);
	event = Mut2.take();                         ASSERT_failure(event);
	                                             ASSERT(!Tsk3);
	        Tsk3.startFrom(proc3);               ASSERT(!Tsk3);
	event = Tsk3.join();                         ASSERT_success(event);
	event = Mut2.give();                         ASSERT_success(event);
	event = Mut2.give();                         ASSERT_failure(event);
	        ThisTask::stop();
}

static void proc1()
{
	unsigned event;

	event = Mut0.take();                         ASSERT_timeout(event);
	event = Mut1.wait();                         ASSERT_success(event);
	event = Mut1.take();                         ASSERT_failure(event);
	                                             ASSERT(!Tsk2);
	        Tsk2.startFrom(proc2);               ASSERT(!Tsk2);
	event = Tsk2.join();                         ASSERT_success(event);
	event = Mut1.give();                         ASSERT_success(event);
	event = Mut1.give();                         ASSERT_failure(event);
	        ThisTask::stop();
}

static void proc0()
{
	unsigned event;

	event = Mut0.wait();                         ASSERT_success(event);
	event = Mut0.take();                         ASSERT_failure(event);
	                                             ASSERT(!Tsk1);
	        Tsk1.startFrom(proc1);               ASSERT(!Tsk1);
	event = Tsk1.join();                         ASSERT_success(event);
	event = Mut0.give();                         ASSERT_success(event);
	event = Mut0.give();                         ASSERT_failure(event);
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
void test_fast_mutex_3()
{
	TEST_Notify();
	TEST_Call();
}
