#include "test.h"

static sem_t *sem;

static void proc3()
{
	int result;

	result = sem_wait(sem);                       ASSERT_success(result);
	         sem_delete(sem);
	         tsk_stop();
}

static void proc2()
{
	tsk_t *tsk;
	int result;

	result = sem_wait(sem);                       ASSERT_success(result);
	         sem_delete(sem);
	sem    = sem_create(0, semBinary);            ASSERT(sem);
	tsk    = tsk_create(3, proc3);                ASSERT(tsk);
	result = sem_give(sem);                       ASSERT_success(result);
	result = tsk_join(tsk);                       ASSERT_success(result);
	         tsk_stop();
}

static void proc1()
{
	tsk_t *tsk;
	int result;

	result = sem_wait(sem);                       ASSERT_success(result);
	         sem_delete(sem);
	sem    = sem_create(0, semBinary);            ASSERT(sem);
	tsk    = tsk_create(2, proc2);                ASSERT(tsk);
	result = sem_give(sem);                       ASSERT_success(result);
	result = tsk_join(tsk);                       ASSERT_success(result);
	         tsk_stop();
}

static void proc0()
{
	tsk_t *tsk;
	int result;

	result = sem_wait(sem);                       ASSERT_success(result);
	         sem_delete(sem);
	sem    = sem_create(0, semBinary);            ASSERT(sem);
	tsk    = tsk_create(1, proc1);                ASSERT(tsk);
	result = sem_give(sem);                       ASSERT_success(result);
	result = tsk_join(tsk);                       ASSERT_success(result);
	         tsk_stop();
}

static void test()
{
	tsk_t *tsk;
	int result;

	sem    = sem_create(0, semBinary);            ASSERT(sem);
	tsk    = tsk_create(0, proc0);                ASSERT(tsk);
	result = sem_give(sem);                       ASSERT_success(result);
	result = tsk_join(tsk);                       ASSERT_success(result);
}

extern "C"
void test_alloc_3()
{
	TEST_Notify();
	TEST_Call();
}
