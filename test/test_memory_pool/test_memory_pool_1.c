#include "test.h"

static_LST(lst3);
static_MEM(mem3, 1, sizeof(unsigned));

static unsigned sent;

static void proc3()
{
	void * p;
	unsigned received;
	int result;

	result = lst_wait(lst3, &p);                  ASSERT_success(result);
	         received = *(unsigned *)p;           ASSERT(sent == received);
             mem_give(mem3, p);
	result = mem_wait(mem2, &p);                  ASSERT_success(result);
	         *(unsigned *)p = received;
	         lst_give(lst2, p);
	         tsk_stop();
}

static void proc2()
{
	void * p;
	unsigned received;
	int result;
	                                              ASSERT_dead(tsk3);
	         tsk_startFrom(tsk3, proc3);          ASSERT_ready(tsk3);
	result = lst_wait(lst2, &p);                  ASSERT_success(result);
	         received = *(unsigned *)p;           ASSERT(sent == received);
             mem_give(mem2, p);
	result = mem_wait(mem3, &p);                  ASSERT_success(result);
	         *(unsigned *)p = received;
	         lst_give(lst3, p);
	result = lst_wait(lst2, &p);                  ASSERT_success(result);
	         received = *(unsigned *)p;           ASSERT(sent == received);
             mem_give(mem2, p);
	result = mem_wait(mem1, &p);                  ASSERT_success(result);
	        *(unsigned *)p = received;
	         lst_give(lst1, p);
	result = tsk_join(tsk3);                      ASSERT_success(result);
	         tsk_stop();
}

static void proc1()
{
	void * p;
	unsigned received;
	int result;
	                                              ASSERT_dead(tsk2);
	         tsk_startFrom(tsk2, proc2);          ASSERT_ready(tsk2);
	result = lst_wait(lst1, &p);                  ASSERT_success(result);
	         received = *(unsigned *)p;           ASSERT(sent == received);
             mem_give(mem1, p);
	result = mem_wait(mem2, &p);                  ASSERT_success(result);
	         *(unsigned *)p = received;
	         lst_give(lst2, p);
	result = lst_wait(lst1, &p);                  ASSERT_success(result);
	         received = *(unsigned *)p;           ASSERT(sent == received);
             mem_give(mem1, p);
	result = mem_wait(&mem0, &p);                 ASSERT_success(result);
	         *(unsigned *)p = received;
	         lst_give(&lst0, p);
	result = tsk_join(tsk2);                      ASSERT_success(result);
	         tsk_stop();
}

static void proc0()
{
	void * p;
	unsigned received;
	int result;
	                                              ASSERT_dead(tsk1);
	         tsk_startFrom(tsk1, proc1);          ASSERT_ready(tsk1);
	result = lst_wait(&lst0, &p);                 ASSERT_success(result);
	         received = *(unsigned *)p;           ASSERT(sent == received);
             mem_give(&mem0, p);
	result = mem_wait(mem1, &p);                  ASSERT_success(result);
	         *(unsigned *)p = received;
	         lst_give(lst1, p);
	result = lst_wait(&lst0, &p);                 ASSERT_success(result);
	         received = *(unsigned *)p;           ASSERT(sent == received);
             mem_give(&mem0, p);
	result = tsk_join(tsk1);                      ASSERT_success(result);
	         tsk_stop();
}

static void test()
{
	void * p;
	int result;
	                                              ASSERT_dead(&tsk0);
	         tsk_startFrom(&tsk0, proc0);         ASSERT_ready(&tsk0);
	         tsk_yield();
	         tsk_yield();
	result = mem_wait(&mem0, &p);                 ASSERT_success(result);
	         *(unsigned *)p = sent = rand();
	         lst_give(&lst0, p);
	result = tsk_join(&tsk0);                     ASSERT_success(result);
}

void test_memory_pool_1()
{
	TEST_Notify();
	mem_bind(&mem0);
	mem_bind(mem1);
	mem_bind(mem2);
	mem_bind(mem3);
	TEST_Call();
}
