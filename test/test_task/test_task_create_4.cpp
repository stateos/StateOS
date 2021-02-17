#include "test.h"

static void proc()
{
	         tsk_stop();
}

static void test()
{
	int result;

	tsk_t *tsk6 = wrk_create(6, proc, 512, false, true); ASSERT(tsk6);
	result = tsk_join(tsk6);                      ASSERT_success(result);

	tsk_t *tsk7 = tsk_create(7, proc);            ASSERT(tsk7);
	result = tsk_join(tsk7);                      ASSERT_success(result);

	tsk_t *tsk8 = wrk_create(8, proc, 512, false, true); ASSERT(tsk8);
	result = tsk_join(tsk8);                      ASSERT_success(result);

	tsk_t *tsk9 = tsk_create(9, proc);            ASSERT(tsk9);
	result = tsk_join(tsk9);                      ASSERT_success(result);
}

extern "C"
void test_task_create_4()
{
	TEST_Notify();
	TEST_Call();
}
