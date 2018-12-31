#include "test.h"

#define SIZE sizeof(unsigned)

static_MSG(msg3, 2 * SIZE);

static unsigned sent;

static void proc3()
{
	unsigned bytes;
	unsigned event;

 	bytes = msg_wait(msg3, &event, SIZE);        ASSERT(bytes == SIZE);
	                                             ASSERT(event == sent);
	event = msg_give(msg2, &event, SIZE);        ASSERT_success(event);
	        tsk_stop();
}

static void proc2()
{
	unsigned bytes;
	unsigned event;
		                                         ASSERT_dead(tsk3);
	        tsk_startFrom(tsk3, proc3);          ASSERT_ready(tsk3);
 	bytes = msg_wait(msg2, &event, SIZE);        ASSERT(bytes == SIZE);
 	                                             ASSERT(event == sent);
	event = msg_give(msg3, &event, SIZE);        ASSERT_success(event);
 	bytes = msg_wait(msg2, &event, SIZE);        ASSERT(bytes == SIZE);
	                                             ASSERT(event == sent);
	event = msg_give(msg1, &event, SIZE);        ASSERT_success(event);
	event = tsk_join(tsk3);                      ASSERT_success(event);
	        tsk_stop();
}

static void proc1()
{
	unsigned bytes;
	unsigned event;
		                                         ASSERT_dead(tsk2);
	        tsk_startFrom(tsk2, proc2);          ASSERT_ready(tsk2);
 	bytes = msg_wait(msg1, &event, SIZE);        ASSERT(bytes == SIZE);
 	                                             ASSERT(event == sent);
	event = msg_give(msg2, &event, SIZE);        ASSERT_success(event);
 	bytes = msg_wait(msg1, &event, SIZE);        ASSERT(bytes == SIZE);
 	                                             ASSERT(event == sent);
	event = msg_give(&msg0, &event, SIZE);       ASSERT_success(event);
	event = tsk_join(tsk2);                      ASSERT_success(event);
	        tsk_stop();
}

static void proc0()
{
	unsigned bytes;
	unsigned event;
		                                         ASSERT_dead(tsk1);
	        tsk_startFrom(tsk1, proc1);          ASSERT_ready(tsk1);
 	bytes = msg_wait(&msg0, &event, SIZE);       ASSERT(bytes == SIZE);
 	                                             ASSERT(event == sent);
	event = msg_give(msg1, &event, SIZE);        ASSERT_success(event);
 	bytes = msg_wait(&msg0, &event, SIZE);       ASSERT(bytes == SIZE);
 	                                             ASSERT(event == sent);
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
	event = msg_give(&msg0, &sent, SIZE);        ASSERT_success(event);
	event = tsk_join(&tsk0);                     ASSERT_success(event);
}

extern "C"
void test_message_buffer_2()
{
	TEST_Notify();
	TEST_Call();
}
