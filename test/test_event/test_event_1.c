#include "test.h"

static_EVT(evt3);

static unsigned sent;

static void proc3()
{
	unsigned received;
	unsigned event;

 	event = evt_wait(evt3, &received);           assert_success(event);
 	                                             assert(received == sent);
	        tsk_stop();
}

static void proc2()
{
	unsigned received;
	unsigned event;
		                                         assert_dead(tsk3);
	        tsk_startFrom(tsk3, proc3);          assert_ready(tsk3);
 	event = evt_wait(evt2, &received);           assert_success(event);
 	                                             assert(received == sent);
	        evt_give(evt3, received);
	event = tsk_join(tsk3);                      assert_success(event);
	        tsk_stop();
}

static void proc1()
{
	unsigned received;
	unsigned event;
		                                         assert_dead(tsk2);
	        tsk_startFrom(tsk2, proc2);          assert_ready(tsk2);
 	event = evt_wait(evt1, &received);           assert_success(event);
 	                                             assert(received == sent);
	        evt_give(evt2, received);
	event = tsk_join(tsk2);                      assert_success(event);
	        tsk_stop();
}

static void proc0()
{
	unsigned event;
		                                         assert_dead(tsk1);
	        tsk_startFrom(tsk1, proc1);          assert_ready(tsk1);
	        sent = rand();
	        evt_give(evt1, sent);
	event = tsk_join(tsk1);                      assert_success(event);
	        tsk_stop();
}

static void test()
{
	unsigned event;
		                                         assert_dead(&tsk0);
	        tsk_startFrom(&tsk0, proc0);
	event = tsk_join(&tsk0);                     assert_success(event);
}

void test_event_1()
{
	TEST_Notify();
	TEST_Call();
}
