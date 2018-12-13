#include "test.h"

static_LST(lst3);
static_MEM(mem3, 1, sizeof(unsigned));

static unsigned sent;

static void proc3()
{
	void   * p;
	unsigned received;
	unsigned event;

 	event = lst_wait(lst3, &p);                  assert_success(event);
	        received = *(unsigned *)p;           assert(sent == received);
            mem_give(mem3, p);
 	event = mem_wait(mem2, &p);                  assert_success(event);
	        *(unsigned *)p = received;
	        lst_give(lst2, p);
	        tsk_stop();
}

static void proc2()
{
	void   * p;
	unsigned received;
	unsigned event;
		                                         assert_dead(tsk3);
	        tsk_startFrom(tsk3, proc3);          assert_ready(tsk3);
 	event = lst_wait(lst2, &p);                  assert_success(event);
	        received = *(unsigned *)p;           assert(sent == received);
            mem_give(mem2, p);
 	event = mem_wait(mem3, &p);                  assert_success(event);
	        *(unsigned *)p = received;
	        lst_give(lst3, p);
 	event = lst_wait(lst2, &p);                  assert_success(event);
	        received = *(unsigned *)p;           assert(sent == received);
            mem_give(mem2, p);
 	event = mem_wait(mem1, &p);                  assert_success(event);
	        *(unsigned *)p = received;
	        lst_give(lst1, p);
	event = tsk_join(tsk3);                      assert_success(event);
	        tsk_stop();
}

static void proc1()
{
	void   * p;
	unsigned received;
	unsigned event;
		                                         assert_dead(tsk2);
	        tsk_startFrom(tsk2, proc2);          assert_ready(tsk2);
 	event = lst_wait(lst1, &p);                  assert_success(event);
	        received = *(unsigned *)p;           assert(sent == received);
            mem_give(mem1, p);
 	event = mem_wait(mem2, &p);                  assert_success(event);
	        *(unsigned *)p = received;
	        lst_give(lst2, p);
 	event = lst_wait(lst1, &p);                  assert_success(event);
	        received = *(unsigned *)p;           assert(sent == received);
            mem_give(mem1, p);
 	event = mem_wait(&mem0, &p);                 assert_success(event);
	        *(unsigned *)p = received;
	        lst_give(&lst0, p);
	event = tsk_join(tsk2);                      assert_success(event);
	        tsk_stop();
}

static void proc0()
{
	void   * p;
	unsigned received;
	unsigned event;
		                                         assert_dead(tsk1);
	        tsk_startFrom(tsk1, proc1);          assert_ready(tsk1);
 	event = lst_wait(&lst0, &p);                 assert_success(event);
	        received = *(unsigned *)p;           assert(sent == received);
            mem_give(&mem0, p);
 	event = mem_wait(mem1, &p);                  assert_success(event);
	        *(unsigned *)p = received;
	        lst_give(lst1, p);
 	event = lst_wait(&lst0, &p);                 assert_success(event);
	        received = *(unsigned *)p;           assert(sent == received);
            mem_give(&mem0, p);
	event = tsk_join(tsk1);                      assert_success(event);
	        tsk_stop();
}

static void test()
{
	void   * p;
	unsigned event;
		                                         assert_dead(&tsk0);
	        tsk_startFrom(&tsk0, proc0);         assert_ready(&tsk0);
	        tsk_yield();
	        tsk_yield();
 	event = mem_wait(&mem0, &p);                 assert_success(event);
	        *(unsigned *)p = sent = rand();
	        lst_give(&lst0, p);
	event = tsk_join(&tsk0);                     assert_success(event);
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
