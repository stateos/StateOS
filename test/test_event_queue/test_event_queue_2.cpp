#include "test.h"

static_EVQ(evq3, 1);

static unsigned sent;

static void proc3()
{
	unsigned received;
	unsigned event;

 	event = evq_wait(evq3, &received);           ASSERT_success(event);
 	                                             ASSERT(received == sent);
	event = evq_give(evq2, received);            ASSERT_success(event);
	        tsk_stop();
}

static void proc2()
{
	unsigned received;
	unsigned event;
		                                         ASSERT_dead(tsk3);
	        tsk_startFrom(tsk3, proc3);          ASSERT_ready(tsk3);
 	event = evq_wait(evq2, &received);           ASSERT_success(event);
 	                                             ASSERT(received == sent);
	event = evq_give(evq3, received);            ASSERT_success(event);
 	event = evq_wait(evq2, &received);           ASSERT_success(event);
 	                                             ASSERT(received == sent);
	event = evq_give(evq1, received);            ASSERT_success(event);
	event = tsk_join(tsk3);                      ASSERT_success(event);
	        tsk_stop();
}

static void proc1()
{
	unsigned received;
	unsigned event;
		                                         ASSERT_dead(tsk2);
	        tsk_startFrom(tsk2, proc2);          ASSERT_ready(tsk2);
 	event = evq_wait(evq1, &received);           ASSERT_success(event);
 	                                             ASSERT(received == sent);
	event = evq_give(evq2, received);            ASSERT_success(event);
 	event = evq_wait(evq1, &received);           ASSERT_success(event);
 	                                             ASSERT(received == sent);
	event = evq_give(&evq0, received);           ASSERT_success(event);
	event = tsk_join(tsk2);                      ASSERT_success(event);
	        tsk_stop();
}

static void proc0()
{
	unsigned received;
	unsigned event;
		                                         ASSERT_dead(tsk1);
	        tsk_startFrom(tsk1, proc1);          ASSERT_ready(tsk1);
 	event = evq_wait(&evq0, &received);          ASSERT_success(event);
 	                                             ASSERT(received == sent);
	event = evq_give(evq1, received);            ASSERT_success(event);
 	event = evq_wait(&evq0, &received);          ASSERT_success(event);
 	                                             ASSERT(received == sent);
	event = tsk_join(tsk1);                      ASSERT_success(event);
	        tsk_stop();
}

static void test()
{
	unsigned event;
		                                         ASSERT_dead(&tsk0);
	        tsk_startFrom(&tsk0, proc0);         ASSERT_ready(&tsk0);
	event = evq_give(&evq0, sent = rand());      ASSERT_success(event);
	event = tsk_join(&tsk0);                     ASSERT_success(event);
}

extern "C"
void test_event_queue_2()
{
	TEST_Notify();
	TEST_Call();
}
