#include "test.h"

static_EVT(evt3);

static unsigned sent;

static void proc3()
{
	unsigned received;
	int result;

	result = evt_wait(evt3, &received);           ASSERT_success(result);
	                                              ASSERT(received == sent);
	         tsk_stop();
}

static void proc2()
{
	unsigned received;
	int result;
	                                              ASSERT_dead(tsk3);
	         tsk_startFrom(tsk3, proc3);          ASSERT_ready(tsk3);
	result = evt_wait(evt2, &received);           ASSERT_success(result);
	                                              ASSERT(received == sent);
	         evt_give(evt3, received);
	result = tsk_join(tsk3);                      ASSERT_success(result);
	         tsk_stop();
}

static void proc1()
{
	unsigned received;
	int result;
	                                              ASSERT_dead(tsk2);
	         tsk_startFrom(tsk2, proc2);          ASSERT_ready(tsk2);
	result = evt_wait(evt1, &received);           ASSERT_success(result);
	                                              ASSERT(received == sent);
	         evt_give(evt2, received);
	result = tsk_join(tsk2);                      ASSERT_success(result);
	         tsk_stop();
}

static void proc0()
{
	unsigned received;
	int result;
	                                              ASSERT_dead(tsk1);
	         tsk_startFrom(tsk1, proc1);          ASSERT_ready(tsk1);
	result = evt_wait(&evt0, &received);          ASSERT_success(result);
	                                              ASSERT(received == sent);
	         evt_give(evt1, received);
	result = tsk_join(tsk1);                      ASSERT_success(result);
	         tsk_stop();
}

static void test()
{
	int result;
	                                              ASSERT_dead(&tsk0);
	         tsk_startFrom(&tsk0, proc0);         ASSERT_ready(&tsk0);
	         tsk_yield();
	         tsk_yield();
	         evt_give(&evt0, sent = rand());
	result = tsk_join(&tsk0);                     ASSERT_success(result);
}

extern "C"
void test_event_2()
{
	TEST_Notify();
	TEST_Call();
}
