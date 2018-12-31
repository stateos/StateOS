#include "test.h"

static_JOB(job3, 1);

static int counter;

static void proc()
{
	        sys_lock();
	        {
		        counter++;
	        }
	        sys_unlock();
}

static void proc3()
{
	unsigned event;

	event = job_wait(job3);                      ASSERT_success(event);
	                                             ASSERT(counter == 4);
	event = job_give(job2, proc);                ASSERT_success(event);
	                                             
	        tsk_stop();
}

static void proc2()
{
	unsigned event;
		                                         ASSERT_dead(tsk3);
	        tsk_startFrom(tsk3, proc3);          ASSERT_ready(tsk3);
	event = job_wait(job2);                      ASSERT_success(event);
	                                             ASSERT(counter == 3);
	event = job_give(job3, proc);                ASSERT_success(event);
	event = job_wait(job2);                      ASSERT_success(event);
	                                             ASSERT(counter == 5);
	event = job_give(job1, proc);                ASSERT_success(event);
	event = tsk_join(tsk3);                      ASSERT_success(event);
	        tsk_stop();
}

static void proc1()
{
	unsigned event;
		                                         ASSERT_dead(tsk2);
	        tsk_startFrom(tsk2, proc2);          ASSERT_ready(tsk2);
	event = job_wait(job1);                      ASSERT_success(event);
	                                             ASSERT(counter == 2);
	event = job_give(job2, proc);                ASSERT_success(event);
	event = job_wait(job1);                      ASSERT_success(event);
	                                             ASSERT(counter == 6);
	event = job_give(&job0, proc);               ASSERT_success(event);
	event = tsk_join(tsk2);                      ASSERT_success(event);
	        tsk_stop();
}

static void proc0()
{
	unsigned event;
		                                         ASSERT_dead(tsk1);
	        tsk_startFrom(tsk1, proc1);          ASSERT_ready(tsk1);
	event = job_wait(&job0);                     ASSERT_success(event);
	                                             ASSERT(counter == 1);
	event = job_give(job1, proc);                ASSERT_success(event);
	event = job_wait(&job0);                     ASSERT_success(event);
	                                             ASSERT(counter == 7);
	event = tsk_join(tsk1);                      ASSERT_success(event);
	        tsk_stop();
}

static void test()
{
	unsigned event;
		                                         ASSERT_dead(&tsk0);
	        tsk_startFrom(&tsk0, proc0);         ASSERT_ready(&tsk0);
	        tsk_yield();
	        tsk_yield();
	        counter = 0;
	event = job_give(&job0, proc);               ASSERT_success(event);
	event = tsk_join(&tsk0);                     ASSERT_success(event);
}

void test_job_queue_1()
{
	TEST_Notify();
	TEST_Call();
}
