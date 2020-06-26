#include "test.h"

static_SEM(sem3, 0, semBinary);

static void proc3()
{
	int result;

	result = sem_wait(sem3);                      ASSERT_success(result);
	result = sem_give(sem2);                      ASSERT_success(result);
	         tsk_stop();
}

static void proc2()
{
	int result;
	                                              ASSERT_dead(tsk3);
	         tsk_startFrom(tsk3, proc3);          ASSERT_ready(tsk3);
	result = sem_wait(sem2);                      ASSERT_success(result);
	result = sem_give(sem3);                      ASSERT_success(result);
	result = sem_wait(sem2);                      ASSERT_success(result);
	result = sem_give(sem1);                      ASSERT_success(result);
	result = tsk_join(tsk3);                      ASSERT_success(result);
	         tsk_stop();
}

static void proc1()
{
	int result;
	                                              ASSERT_dead(tsk2);
	         tsk_startFrom(tsk2, proc2);          ASSERT_ready(tsk2);
	result = sem_wait(sem1);                      ASSERT_success(result);
	result = sem_give(sem2);                      ASSERT_success(result);
	result = sem_wait(sem1);                      ASSERT_success(result);
	result = sem_give(&sem0);                     ASSERT_success(result);
	result = tsk_join(tsk2);                      ASSERT_success(result);
	         tsk_stop();
}

static void proc0()
{
	int result;
	                                              ASSERT_dead(tsk1);
	         tsk_startFrom(tsk1, proc1);          ASSERT_ready(tsk1);
	result = sem_wait(&sem0);                     ASSERT_success(result);
	result = sem_give(sem1);                      ASSERT_success(result);
	result = sem_wait(&sem0);                     ASSERT_success(result);
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
	result = sem_give(&sem0);                     ASSERT_success(result);
	result = tsk_join(&tsk0);                     ASSERT_success(result);
}

extern "C"
void test_semaphore_2()
{
	TEST_Notify();
	TEST_Call();
}
