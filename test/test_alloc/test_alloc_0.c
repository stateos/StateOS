#include "test.h"

#define SIZE 256

static void proc2()
{
	size_t len1 = rand() % (SIZE) + 1;
	size_t len2 = rand() % (SIZE) + 1;
	void * buf1 = malloc(len1);                  assert(buf1);
	void * buf2 = sys_alloc(len2);               assert(buf2);
	memset(buf1, 0xFF, len1);
	memset(buf2, 0xFF, len2);
	free(buf1);
	sys_free(buf2);
	tsk_stop();
}

static void proc1()
{
	stk_t  * stk;
	tsk_t  * tsk;
	unsigned event;
	tsk = malloc(sizeof(tsk_t));                 assert(tsk);
	stk = sys_alloc(OS_STACK_SIZE);              assert(stk);
	tsk_init(tsk, 2, proc2, stk, OS_STACK_SIZE);
	event = tsk_join(tsk);                       assert_success(event);
	free(tsk);
	sys_free(stk);
	tsk_stop();
}

static void test()
{
	tsk_t  * tsk;
	unsigned event;

	tsk = tsk_new(1, proc1);                     assert(tsk);
	event = tsk_join(tsk);                       assert_success(event);
}

void test_alloc_0()
{
	TEST_Notify();
	TEST_Call();
}
