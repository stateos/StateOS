#include "test.h"

#define SIZE sizeof(unsigned)

static_STM(stm3, SIZE);

static unsigned sent;

static void proc3()
{
	unsigned event;
	unsigned value;
	unsigned read = SIZE;

 	event = stm_wait(stm3, &value, SIZE, &read); ASSERT_success(event);
	                                             ASSERT(read == SIZE);
	                                             ASSERT(value == sent);
	event = stm_give(stm2, &value, SIZE);        ASSERT_success(event);
	        tsk_stop();
}

static void proc2()
{
	unsigned event;
	unsigned value;
	unsigned read = SIZE;
		                                         ASSERT_dead(tsk3);
	        tsk_startFrom(tsk3, proc3);          ASSERT_ready(tsk3);
 	event = stm_wait(stm2, &value, SIZE, &read); ASSERT_success(event);
	                                             ASSERT(read == SIZE);
	                                             ASSERT(value == sent);
	event = stm_give(stm3, &value, SIZE);        ASSERT_success(event);
 	event = stm_wait(stm2, &value, SIZE, &read); ASSERT_success(event);
	                                             ASSERT(read == SIZE);
	                                             ASSERT(value == sent);
	event = stm_give(stm1, &value, SIZE);        ASSERT_success(event);
	event = tsk_join(tsk3);                      ASSERT_success(event);
	        tsk_stop();
}

static void proc1()
{
	unsigned event;
	unsigned value;
	unsigned read = SIZE;
		                                         ASSERT_dead(tsk2);
	        tsk_startFrom(tsk2, proc2);          ASSERT_ready(tsk2);
 	event = stm_wait(stm1, &value, SIZE, &read); ASSERT_success(event);
	                                             ASSERT(read == SIZE);
	                                             ASSERT(value == sent);
	event = stm_give(stm2, &value, SIZE);        ASSERT_success(event);
 	event = stm_wait(stm1, &value, SIZE, &read); ASSERT_success(event);
	                                             ASSERT(read == SIZE);
	                                             ASSERT(value == sent);
	event = stm_give(&stm0, &value, SIZE);       ASSERT_success(event);
	event = tsk_join(tsk2);                      ASSERT_success(event);
	        tsk_stop();
}

static void proc0()
{
	unsigned event;
	unsigned value;
	unsigned read = SIZE;
		                                         ASSERT_dead(tsk1);
	        tsk_startFrom(tsk1, proc1);          ASSERT_ready(tsk1);
 	event = stm_wait(&stm0, &value, SIZE, &read);ASSERT_success(event);
	                                             ASSERT(read == SIZE);
	                                             ASSERT(value == sent);
	event = stm_give(stm1, &value, SIZE);        ASSERT_success(event);
 	event = stm_wait(&stm0, &value, SIZE, &read);ASSERT_success(event);
	                                             ASSERT(read == SIZE);
	                                             ASSERT(value == sent);
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
	event = stm_give(&stm0, &sent, SIZE);        ASSERT_success(event);
	event = tsk_join(&tsk0);                     ASSERT_success(event);
}

extern "C"
void test_stream_buffer_2()
{
	TEST_Notify();
	TEST_Call();
}
