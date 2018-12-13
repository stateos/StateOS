#include "test.h"

#define SIZE sizeof(unsigned)

static_STM(stm3, SIZE);

static unsigned sent;

static void proc3()
{
	unsigned bytes;
	unsigned event;

 	bytes = stm_wait(stm3, &event, SIZE);        assert(bytes == SIZE);
	                                             assert(event == sent);
	event = stm_give(stm2, &event, SIZE);        assert_success(event);
	        tsk_stop();
}

static void proc2()
{
	unsigned bytes;
	unsigned event;
		                                         assert_dead(tsk3);
	        tsk_startFrom(tsk3, proc3);          assert_ready(tsk3);
 	bytes = stm_wait(stm2, &event, SIZE);        assert(bytes == SIZE);
 	                                             assert(event == sent);
	event = stm_give(stm3, &event, SIZE);        assert_success(event);
 	bytes = stm_wait(stm2, &event, SIZE);        assert(bytes == SIZE);
	                                             assert(event == sent);
	event = stm_give(stm1, &event, SIZE);        assert_success(event);
	event = tsk_join(tsk3);                      assert_success(event);
	        tsk_stop();
}

static void proc1()
{
	unsigned bytes;
	unsigned event;
		                                         assert_dead(tsk2);
	        tsk_startFrom(tsk2, proc2);          assert_ready(tsk2);
 	bytes = stm_wait(stm1, &event, SIZE);        assert(bytes == SIZE);
 	                                             assert(event == sent);
	event = stm_give(stm2, &event, SIZE);        assert_success(event);
 	bytes = stm_wait(stm1, &event, SIZE);        assert(bytes == SIZE);
 	                                             assert(event == sent);
	event = stm_give(&stm0, &event, SIZE);       assert_success(event);
	event = tsk_join(tsk2);                      assert_success(event);
	        tsk_stop();
}

static void proc0()
{
	unsigned bytes;
	unsigned event;
		                                         assert_dead(tsk1);
	        tsk_startFrom(tsk1, proc1);          assert_ready(tsk1);
 	bytes = stm_wait(&stm0, &event, SIZE);       assert(bytes == SIZE);
 	                                             assert(event == sent);
	event = stm_give(stm1, &event, SIZE);        assert_success(event);
 	bytes = stm_wait(&stm0, &event, SIZE);       assert(bytes == SIZE);
 	                                             assert(event == sent);
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
	event = stm_give(&stm0, &sent, SIZE);        assert_success(event);
	event = tsk_join(&tsk0);                     assert_success(event);
}

void test_stream_buffer_1()
{
	TEST_Notify();
	TEST_Call();
}
