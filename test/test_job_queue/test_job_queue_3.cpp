#include "test.h"

static auto Job0 = JobQueueT<1>();
static auto Job1 = JobQueueT<1>();
static auto Job2 = JobQueueT<1>();
static auto Job3 = JobQueueT<1>();

static int counter;

static void proc()
{
	CriticalSection cs;

	        counter++;
}

static void proc3()
{
	unsigned event;

	event = Job3.wait();                         assert_success(event);
	                                             assert(counter == 4);
	event = Job2.give(proc);                     assert_success(event);
	        ThisTask::stop();
}

static void proc2()
{
	unsigned event;
		                                         assert(!Tsk3);
	        Tsk3.startFrom(proc3);               assert(!!Tsk3);
	event = Job2.wait();                         assert_success(event);
	                                             assert(counter == 3);
	event = Job3.give(proc);                     assert_success(event);
	event = Job2.wait();                         assert_success(event);
	                                             assert(counter == 5);
	event = Job1.give(proc);                     assert_success(event);
	event = Tsk3.join();                         assert_success(event);
	        ThisTask::stop();
}

static void proc1()
{
	unsigned event;
		                                         assert(!Tsk2);
	        Tsk2.startFrom(proc2);               assert(!!Tsk2);
	event = Job1.wait();                         assert_success(event);
	                                             assert(counter == 2);
	event = Job2.give(proc);                     assert_success(event);
	event = Job1.wait();                         assert_success(event);
	                                             assert(counter == 6);
	event = Job0.give(proc);                     assert_success(event);
	event = Tsk2.join();                         assert_success(event);
	        ThisTask::stop();
}

static void proc0()
{
	unsigned event;
		                                         assert(!Tsk1);
	        Tsk1.startFrom(proc1);               assert(!!Tsk1);
	event = Job0.wait();                         assert_success(event);
	                                             assert(counter == 1);
	event = Job1.give(proc);                     assert_success(event);
	event = Job0.wait();                         assert_success(event);
	                                             assert(counter == 7);
	event = Tsk1.join();                         assert_success(event);
	        ThisTask::stop();
}

static void test()
{
	unsigned event;
	                                             assert(!Tsk0);
	        Tsk0.startFrom(proc0);               assert(!!Tsk0);
	        ThisTask::yield();
	        ThisTask::yield();
	        counter = 0;
	event = Job0.give(proc);                     assert_success(event);
	event = Tsk0.join();                         assert_success(event);
}

extern "C"
void test_job_queue_3()
{
	TEST_Notify();
	TEST_Call();
}
