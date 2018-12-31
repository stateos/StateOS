#include "test.h"

static_BOX(box3, 1, sizeof(unsigned));

static unsigned sent;

static void proc3()
{
	unsigned received;
	unsigned event;

 	event = box_wait(box3, &received);           ASSERT_success(event);
 	                                             ASSERT(received == sent);
	event = box_give(box2, &received);           ASSERT_success(event);
	        tsk_stop();
}

static void proc2()
{
	unsigned received;
	unsigned event;
		                                         ASSERT_dead(tsk3);
	        tsk_startFrom(tsk3, proc3);          ASSERT_ready(tsk3);
 	event = box_wait(box2, &received);           ASSERT_success(event);
 	                                             ASSERT(received == sent);
	event = box_give(box3, &received);           ASSERT_success(event);
 	event = box_wait(box2, &received);           ASSERT_success(event);
 	                                             ASSERT(received == sent);
	event = box_give(box1, &received);           ASSERT_success(event);
	event = tsk_join(tsk3);                      ASSERT_success(event);
	        tsk_stop();
}

static void proc1()
{
	unsigned received;
	unsigned event;
		                                         ASSERT_dead(tsk2);
	        tsk_startFrom(tsk2, proc2);          ASSERT_ready(tsk2);
 	event = box_wait(box1, &received);           ASSERT_success(event);
 	                                             ASSERT(received == sent);
	event = box_give(box2, &received);           ASSERT_success(event);
 	event = box_wait(box1, &received);           ASSERT_success(event);
 	                                             ASSERT(received == sent);
	event = box_give(&box0, &received);          ASSERT_success(event);
	event = tsk_join(tsk2);                      ASSERT_success(event);
	        tsk_stop();
}

static void proc0()
{
	unsigned received;
	unsigned event;
		                                         ASSERT_dead(tsk1);
	        tsk_startFrom(tsk1, proc1);          ASSERT_ready(tsk1);
 	event = box_wait(&box0, &received);          ASSERT_success(event);
 	                                             ASSERT(received == sent);
	event = box_give(box1, &received);           ASSERT_success(event);
 	event = box_wait(&box0, &received);          ASSERT_success(event);
 	                                             ASSERT(received == sent);
	event = tsk_join(tsk1);                      ASSERT_success(event);
	        tsk_stop();
}

static void test()
{
	unsigned event;
		                                         ASSERT_dead(&tsk0);
	        tsk_startFrom(&tsk0, proc0);         ASSERT_ready(&tsk0);
	        tsk_yield();
	        tsk_yield();
	        sent = rand();
	event = box_give(&box0, &sent);              ASSERT_success(event);
	event = tsk_join(&tsk0);                     ASSERT_success(event);
}

void test_mailbox_queue_1()
{
	TEST_Notify();
	TEST_Call();
}
