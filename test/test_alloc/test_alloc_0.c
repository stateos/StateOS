#include "test.h"

#define SIZE 256

static void proc2()
{
	size_t len1 = rand() % (SIZE) + 1;
	size_t len2 = rand() % (SIZE) + 1;
	size_t len3 = rand() % (SIZE) + 1;
	size_t len4 = rand() % (SIZE) + 1;
	void * buf1 = malloc(len1);                  ASSERT(buf1);
	void * buf2 = sys_alloc(len2);               ASSERT(buf2);
	memset(buf1, 0xFF, len1);
	memset(buf2, 0xFF, len2);
	       buf1 = realloc(buf1, len3);           ASSERT(buf1);
	       buf2 = sys_realloc(buf2, len4);       ASSERT(buf2);
	memset(buf1, 0xFF, len3);
	memset(buf2, 0xFF, len4);
	free(buf1);
	sys_free(buf2);
	tsk_stop();
}

static void proc1()
{
	stk_t  * stk;
	tsk_t  * tsk;
	unsigned event;
	tsk = malloc(sizeof(tsk_t));                 ASSERT(tsk);
	stk = sys_alloc(OS_STACK_SIZE);              ASSERT(stk);
	tsk_init(tsk, 2, proc2, stk, OS_STACK_SIZE); ASSERT_dead(tsk);
	event = tsk_join(tsk);                       ASSERT_success(event);
	free(tsk);
	sys_free(stk);
	tsk_stop();
}

static void test()
{
	tsk_t  * tsk;
	unsigned event;

	tsk = tsk_new(1, proc1);                     ASSERT(tsk);
	event = tsk_join(tsk);                       ASSERT_success(event);
}

void test_alloc_0()
{
	TEST_Notify();
	TEST_Call();
}
