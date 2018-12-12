#include "test.h"

static auto Mut0 = FastMutex();
static auto Mut1 = FastMutex();
static auto Mut2 = FastMutex();
static auto Mut3 = FastMutex();

static void proc3()
{
	unsigned event;

	event = Mut2.take();                         assert_timeout(event);
	event = Mut3.wait();                         assert_success(event);
	event = Mut3.take();                         assert_failure(event);
	event = Mut3.give();                         assert_success(event);
	        ThisTask::stop();
}

static void proc2()
{
	unsigned event;

	event = Mut1.take();                         assert_timeout(event);
	event = Mut2.wait();                         assert_success(event);
	event = Mut2.take();                         assert_failure(event);
	                                             assert(!Tsk3);
	        Tsk3.startFrom(proc3);               assert(!Tsk3);
	event = Tsk3.join();                         assert_success(event);
	event = Mut2.give();                         assert_success(event);
	        ThisTask::stop();
}

static void proc1()
{
	unsigned event;

	event = Mut0.take();                         assert_timeout(event);
	event = Mut1.wait();                         assert_success(event);
	event = Mut1.take();                         assert_failure(event);
	                                             assert(!Tsk2);
	        Tsk2.startFrom(proc2);               assert(!Tsk2);
	event = Tsk2.join();                         assert_success(event);
	event = Mut1.give();                         assert_success(event);
	        ThisTask::stop();
}

static void proc0()
{
	unsigned event;

	event = Mut0.wait();                         assert_success(event);
	event = Mut0.take();                         assert_failure(event);
	                                             assert(!Tsk1);
	        Tsk1.startFrom(proc1);               assert(!Tsk1);
	event = Tsk1.join();                         assert_success(event);
	event = Mut0.give();                         assert_success(event);
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
void test_fast_mutex_3()
{
	TEST_Notify();
	TEST_Call();
}
