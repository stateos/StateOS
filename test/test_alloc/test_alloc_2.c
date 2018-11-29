#include "test.h"

static sem_t *sem;

static void proc3()
{
	tsk_stop();
}

static void proc2()
{
	tsk_t  * tsk;
	unsigned event;
	event = sem_wait(sem);                       assert_success(event);
	sem_delete(sem);
	tsk = tsk_create(3, proc3);                  assert(tsk);
	event = tsk_join(tsk);                       assert_success(event);
	tsk_stop();
}

static void proc1()
{
	tsk_t  * tsk;
	unsigned event;
	sem = sem_create(0, semBinary);              assert(sem);
	tsk = tsk_create(2, proc2);                  assert(tsk);
	event = sem_give(sem);                       assert_success(event);
	event = tsk_join(tsk);                       assert_success(event);
	tsk_stop();
}

static void test()
{
	tsk_t  * tsk;
	unsigned event;

	tsk = tsk_create(1, proc1);                  assert(tsk);
	event = tsk_join(tsk);                       assert_success(event);
}

void test_alloc_2()
{
	int i;
	TEST_Notify();
	for (i = 0; i < PASS; i++)
		test();
}
