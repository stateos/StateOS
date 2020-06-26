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
	int result;

	result = Job3.wait();                         ASSERT_success(result);
	                                              ASSERT(counter == 4);
	result = Job2.give(proc);                     ASSERT_success(result);
	         ThisTask::stop();
}

static void proc2()
{
	int result;
	                                              ASSERT(!Tsk3);
	         Tsk3.startFrom(proc3);               ASSERT(!!Tsk3);
	result = Job2.wait();                         ASSERT_success(result);
	                                              ASSERT(counter == 3);
	result = Job3.give(proc);                     ASSERT_success(result);
	result = Job2.wait();                         ASSERT_success(result);
	                                              ASSERT(counter == 5);
	result = Job1.give(proc);                     ASSERT_success(result);
	result = Tsk3.join();                         ASSERT_success(result);
	         ThisTask::stop();
}

static void proc1()
{
	int result;
	                                              ASSERT(!Tsk2);
	         Tsk2.startFrom(proc2);               ASSERT(!!Tsk2);
	result = Job1.wait();                         ASSERT_success(result);
	                                              ASSERT(counter == 2);
	result = Job2.give(proc);                     ASSERT_success(result);
	result = Job1.wait();                         ASSERT_success(result);
	                                              ASSERT(counter == 6);
	result = Job0.give(proc);                     ASSERT_success(result);
	result = Tsk2.join();                         ASSERT_success(result);
	         ThisTask::stop();
}

static void proc0()
{
	int result;
	                                              ASSERT(!Tsk1);
	         Tsk1.startFrom(proc1);               ASSERT(!!Tsk1);
	result = Job0.wait();                         ASSERT_success(result);
	                                              ASSERT(counter == 1);
	result = Job1.give(proc);                     ASSERT_success(result);
	result = Job0.wait();                         ASSERT_success(result);
	                                              ASSERT(counter == 7);
	result = Tsk1.join();                         ASSERT_success(result);
	         ThisTask::stop();
}

static void test()
{
	int result;
	                                              ASSERT(!Tsk0);
	         Tsk0.startFrom(proc0);               ASSERT(!!Tsk0);
	         ThisTask::yield();
	         ThisTask::yield();
	         counter = 0;
	result = Job0.give(proc);                     ASSERT_success(result);
	result = Tsk0.join();                         ASSERT_success(result);
}

extern "C"
void test_job_queue_3()
{
	TEST_Notify();
	TEST_Call();
}
