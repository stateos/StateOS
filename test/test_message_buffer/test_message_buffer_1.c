#include "test.h"

#define SIZE sizeof(unsigned)

static_MSG(msg3, 2 * SIZE);

static unsigned sent;

static void proc3()
{
	unsigned bytes;
	unsigned event;

 	bytes = msg_wait(msg3, &event, SIZE);        assert(bytes == SIZE);
	                                             assert(event == sent);
	event = msg_give(msg2, &event, SIZE);        assert_success(event);
	        tsk_stop();
}

static void proc2()
{
	unsigned bytes;
	unsigned event;
		                                         assert_dead(tsk3);
	        tsk_startFrom(tsk3, proc3);
 	bytes = msg_wait(msg2, &event, SIZE);        assert(bytes == SIZE);
 	                                             assert(event == sent);
	event = msg_give(msg3, &event, SIZE);        assert_success(event);
 	bytes = msg_wait(msg2, &event, SIZE);        assert(bytes == SIZE);
	                                             assert(event == sent);
	event = msg_give(msg1, &event, SIZE);        assert_success(event);
	event = tsk_join(tsk3);                      assert_success(event);
	        tsk_stop();
}

static void proc1()
{
	unsigned bytes;
	unsigned event;
		                                         assert_dead(tsk2);
	        tsk_startFrom(tsk2, proc2);
 	bytes = msg_wait(msg1, &event, SIZE);        assert(bytes == SIZE);
 	                                             assert(event == sent);
	event = msg_give(msg2, &event, SIZE);        assert_success(event);
 	bytes = msg_wait(msg1, &event, SIZE);        assert(bytes == SIZE);
 	                                             assert(event == sent);
	event = msg_give(&msg0, &event, SIZE);       assert_success(event);
	event = tsk_join(tsk2);                      assert_success(event);
	        tsk_stop();
}

static void proc0()
{
	unsigned bytes;
	unsigned event;
		                                         assert_dead(tsk1);
	        tsk_startFrom(tsk1, proc1);
	        sent = rand();
	event = msg_give(msg1, &sent, SIZE);         assert_success(event);
 	bytes = msg_wait(&msg0, &event, SIZE);       assert(bytes == SIZE);
 	                                             assert(event == sent);
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

void test_message_buffer_1()
{
	TEST_Notify();
	TEST_Call();
}
