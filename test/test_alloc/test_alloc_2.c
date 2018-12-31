#include "test.h"

static sem_t *sem;

static void proc3()
{
	unsigned event;

	event = sem_wait(sem);                       ASSERT_success(event);
	        sem_delete(sem);
	        tsk_stop();
}

static void proc2()
{
	tsk_t  * tsk;
	unsigned event;

	event = sem_wait(sem);                       ASSERT_success(event);
	        sem_delete(sem);
	sem   = sem_create(0, semBinary);            ASSERT(sem);
	tsk   = tsk_create(3, proc3);                ASSERT(tsk);
	event = sem_give(sem);                       ASSERT_success(event);
	event = tsk_join(tsk);                       ASSERT_success(event);
	        tsk_stop();
}

static void proc1()
{
	tsk_t  * tsk;
	unsigned event;

	event = sem_wait(sem);                       ASSERT_success(event);
	        sem_delete(sem);
	sem   = sem_create(0, semBinary);            ASSERT(sem);
	tsk   = tsk_create(2, proc2);                ASSERT(tsk);
	event = sem_give(sem);                       ASSERT_success(event);
	event = tsk_join(tsk);                       ASSERT_success(event);
	        tsk_stop();
}

static void proc0()
{
	tsk_t  * tsk;
	unsigned event;

	event = sem_wait(sem);                       ASSERT_success(event);
	        sem_delete(sem);
	sem   = sem_create(0, semBinary);            ASSERT(sem);
	tsk   = tsk_create(1, proc1);                ASSERT(tsk);
	event = sem_give(sem);                       ASSERT_success(event);
	event = tsk_join(tsk);                       ASSERT_success(event);
	        tsk_stop();
}

static void test()
{
	tsk_t  * tsk;
	unsigned event;

	sem   = sem_create(0, semBinary);            ASSERT(sem);
	tsk   = tsk_create(0, proc0);                ASSERT(tsk);
	event = sem_give(sem);                       ASSERT_success(event);
	event = tsk_join(tsk);                       ASSERT_success(event);
}

void test_alloc_2()
{
	TEST_Notify();
	TEST_Call();
}
