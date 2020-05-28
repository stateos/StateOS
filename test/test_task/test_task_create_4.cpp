#include "test.h"

static void proc()
{
	        tsk_stop();
}

static void test()
{
	unsigned event;

	tsk_t *tsk6 = wrk_create(6, proc, 512);      ASSERT(tsk6);
	event = tsk_join(tsk6);                      ASSERT_success(event);

	tsk_t *tsk7 = tsk_create(7, proc);           ASSERT(tsk7);
	event = tsk_join(tsk7);                      ASSERT_success(event);

	tsk_t *tsk8 = wrk_create(8, proc, 512);      ASSERT(tsk8);
	event = tsk_join(tsk8);                      ASSERT_success(event);

	tsk_t *tsk9 = tsk_create(9, proc);           ASSERT(tsk9);
	event = tsk_join(tsk9);                      ASSERT_success(event);
}

extern "C"
void test_task_create_4()
{
	TEST_Notify();
	TEST_Call();
}
