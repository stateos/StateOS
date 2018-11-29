#include "test.h"

#define SIZE 256

static void proc2()
{
	size_t len1 = rand() % (SIZE) + 1;
	size_t len2 = rand() % (SIZE) + 1;
	void * buf1 = sys_alloc(len1);               assert(buf1);
	void * buf2 = malloc(len2);                  assert(buf2);
	memset(buf1, 0xFF, len1);
	memset(buf2, 0xFF, len2);
	sys_free(buf1);
	free(buf2);
	tsk_stop();
}

static void proc1()
{
	stk_t  * stk;
	tsk_t  * tsk;
	unsigned event;
	tsk = sys_alloc(sizeof(tsk_t));              assert(tsk);
	stk = malloc(OS_STACK_SIZE);                 assert(stk);
	tsk_init(tsk, 2, proc2, stk, OS_STACK_SIZE);
	event = tsk_join(tsk);                       assert_success(event);
	sys_free(tsk);
	free(stk);
	tsk_stop();
}

static void test()
{
	tsk_t  * tsk;
	unsigned event;

	tsk = tsk_new(1, proc1);                     assert(tsk);
	event = tsk_join(tsk);                       assert_success(event);
}

void test_alloc_1()
{
	int i;
	TEST_Notify();
	for (i = 0; i < PASS; i++)
		test();
}
