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
	int result;

	result = job_wait(job3);                      ASSERT_success(result);
	                                              ASSERT(counter == 4);
	result = job_give(job2, proc);                ASSERT_success(result);
	                                             
	         tsk_stop();
}

static void proc2()
{
	int result;
	                                              ASSERT_dead(tsk3);
	         tsk_startFrom(tsk3, proc3);          ASSERT_ready(tsk3);
	result = job_wait(job2);                      ASSERT_success(result);
	                                              ASSERT(counter == 3);
	result = job_give(job3, proc);                ASSERT_success(result);
	result = job_wait(job2);                      ASSERT_success(result);
	                                              ASSERT(counter == 5);
	result = job_give(job1, proc);                ASSERT_success(result);
	result = tsk_join(tsk3);                      ASSERT_success(result);
	         tsk_stop();
}

static void proc1()
{
	int result;
	                                              ASSERT_dead(tsk2);
	         tsk_startFrom(tsk2, proc2);          ASSERT_ready(tsk2);
	result = job_wait(job1);                      ASSERT_success(result);
	                                              ASSERT(counter == 2);
	result = job_give(job2, proc);                ASSERT_success(result);
	result = job_wait(job1);                      ASSERT_success(result);
	                                              ASSERT(counter == 6);
	result = job_give(&job0, proc);               ASSERT_success(result);
	result = tsk_join(tsk2);                      ASSERT_success(result);
	         tsk_stop();
}

static void proc0()
{
	int result;
	                                              ASSERT_dead(tsk1);
	         tsk_startFrom(tsk1, proc1);          ASSERT_ready(tsk1);
	result = job_wait(&job0);                     ASSERT_success(result);
	                                              ASSERT(counter == 1);
	result = job_give(job1, proc);                ASSERT_success(result);
	result = job_wait(&job0);                     ASSERT_success(result);
	                                              ASSERT(counter == 7);
	result = tsk_join(tsk1);                      ASSERT_success(result);
	         tsk_stop();
}

static void test()
{
	int result;
	                                              ASSERT_dead(&tsk0);
	         tsk_startFrom(&tsk0, proc0);         ASSERT_ready(&tsk0);
	         tsk_yield();
	         tsk_yield();
	         counter = 0;
	result = job_give(&job0, proc);               ASSERT_success(result);
	result = tsk_join(&tsk0);                     ASSERT_success(result);
}

extern "C"
void test_job_queue_2()
{
	TEST_Notify();
	TEST_Call();
}
