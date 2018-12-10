#include "test.h"

static unsigned sent = 0;
static unsigned received = 0;

static void action()
{
	        received = sent;
}

static void proc1()
{
	unsigned event;

	event = job_wait(job1);                      assert_success(event);
	                                             assert(sent == received);
	        tsk_stop();
}

static void proc2()
{
	unsigned event;

	        sent = rand();
	event = job_give(job1, action);              assert_success(event);
	        tsk_stop();
}

static void test()
{
	unsigned event;
		                                         assert_dead(tsk1);
	        tsk_startFrom(tsk1, proc1);
		                                         assert_dead(tsk2);
	        tsk_startFrom(tsk2, proc2);
	event = tsk_join(tsk2);                      assert_success(event);
	event = tsk_join(tsk1);                      assert_success(event);
}

void test_job_queue_1()
{
	TEST_Notify();
	TEST_Call();
}
