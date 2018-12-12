#include "test.h"

static_EVQ(evq3, 1);

static unsigned sent;

static void proc3()
{
	unsigned received;
	unsigned event;

 	event = evq_wait(evq3, &received);           assert_success(event);
 	                                             assert(received == sent);
	event = evq_give(evq2, received);            assert_success(event);
	        tsk_stop();
}

static void proc2()
{
	unsigned received;
	unsigned event;
		                                         assert_dead(tsk3);
	        tsk_startFrom(tsk3, proc3);          assert_ready(tsk3);
 	event = evq_wait(evq2, &received);           assert_success(event);
 	                                             assert(received == sent);
	event = evq_give(evq3, received);            assert_success(event);
 	event = evq_wait(evq2, &received);           assert_success(event);
 	                                             assert(received == sent);
	event = evq_give(evq1, received);            assert_success(event);
	event = tsk_join(tsk3);                      assert_success(event);
	        tsk_stop();
}

static void proc1()
{
	unsigned received;
	unsigned event;
		                                         assert_dead(tsk2);
	        tsk_startFrom(tsk2, proc2);          assert_ready(tsk2);
 	event = evq_wait(evq1, &received);           assert_success(event);
 	                                             assert(received == sent);
	event = evq_give(evq2, received);            assert_success(event);
 	event = evq_wait(evq1, &received);           assert_success(event);
 	                                             assert(received == sent);
	event = evq_give(&evq0, received);           assert_success(event);
	event = tsk_join(tsk2);                      assert_success(event);
	        tsk_stop();
}

static void proc0()
{
	unsigned received;
	unsigned event;
		                                         assert_dead(tsk1);
	        tsk_startFrom(tsk1, proc1);          assert_ready(tsk1);
 	event = evq_wait(&evq0, &received);          assert_success(event);
 	                                             assert(received == sent);
	event = evq_give(evq1, received);            assert_success(event);
 	event = evq_wait(&evq0, &received);          assert_success(event);
 	                                             assert(received == sent);
	event = tsk_join(tsk1);                      assert_success(event);
	        tsk_stop();
}

static void test()
{
	unsigned event;
		                                         assert_dead(&tsk0);
	        tsk_startFrom(&tsk0, proc0);         assert_ready(&tsk0);
	event = evq_give(&evq0, sent = rand());      assert_success(event);
	event = tsk_join(&tsk0);                     assert_success(event);
}

void test_event_queue_1()
{
	TEST_Notify();
	TEST_Call();
}
