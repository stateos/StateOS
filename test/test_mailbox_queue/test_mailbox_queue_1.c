#include "test.h"

static_BOX(box3, 1, sizeof(unsigned));

static unsigned sent;

static void proc3()
{
	unsigned received;
	unsigned event;

 	event = box_wait(box3, &received);           assert_success(event);
 	                                             assert(received == sent);
	event = box_give(box2, &received);           assert_success(event);
	        tsk_stop();
}

static void proc2()
{
	unsigned received;
	unsigned event;
		                                         assert_dead(tsk3);
	        tsk_startFrom(tsk3, proc3);          assert_ready(tsk3);
 	event = box_wait(box2, &received);           assert_success(event);
 	                                             assert(received == sent);
	event = box_give(box3, &received);           assert_success(event);
 	event = box_wait(box2, &received);           assert_success(event);
 	                                             assert(received == sent);
	event = box_give(box1, &received);           assert_success(event);
	event = tsk_join(tsk3);                      assert_success(event);
	        tsk_stop();
}

static void proc1()
{
	unsigned received;
	unsigned event;
		                                         assert_dead(tsk2);
	        tsk_startFrom(tsk2, proc2);          assert_ready(tsk2);
 	event = box_wait(box1, &received);           assert_success(event);
 	                                             assert(received == sent);
	event = box_give(box2, &received);           assert_success(event);
 	event = box_wait(box1, &received);           assert_success(event);
 	                                             assert(received == sent);
	event = box_give(&box0, &received);          assert_success(event);
	event = tsk_join(tsk2);                      assert_success(event);
	        tsk_stop();
}

static void proc0()
{
	unsigned received;
	unsigned event;
		                                         assert_dead(tsk1);
	        tsk_startFrom(tsk1, proc1);          assert_ready(tsk1);
 	event = box_wait(&box0, &received);          assert_success(event);
 	                                             assert(received == sent);
	event = box_give(box1, &received);           assert_success(event);
 	event = box_wait(&box0, &received);          assert_success(event);
 	                                             assert(received == sent);
	event = tsk_join(tsk1);                      assert_success(event);
	        tsk_stop();
}

static void test()
{
	unsigned event;
		                                         assert_dead(&tsk0);
	        tsk_startFrom(&tsk0, proc0);         assert_ready(&tsk0);
	        tsk_yield();
	        tsk_yield();
	        sent = rand();
	event = box_give(&box0, &sent);              assert_success(event);
	event = tsk_join(&tsk0);                     assert_success(event);
}

void test_mailbox_queue_1()
{
	TEST_Notify();
	TEST_Call();
}
