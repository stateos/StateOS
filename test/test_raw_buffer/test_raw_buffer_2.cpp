#include "test.h"

#define SIZE sizeof(unsigned)

static_RAW(raw3, SIZE);

static unsigned sent;

static void proc3()
{
	int result;
	unsigned value;
	unsigned read = SIZE;

	result = raw_wait(raw3, &value, SIZE, &read); ASSERT_success(result);
	                                              ASSERT(read == SIZE);
	                                              ASSERT(value == sent);
	result = raw_give(raw2, &value, SIZE);        ASSERT_success(result);
	         tsk_stop();
}

static void proc2()
{
	int result;
	unsigned value;
	unsigned read = SIZE;
	                                              ASSERT_dead(tsk3);
	         tsk_startFrom(tsk3, proc3);          ASSERT_ready(tsk3);
	result = raw_wait(raw2, &value, SIZE, &read); ASSERT_success(result);
	                                              ASSERT(read == SIZE);
	                                              ASSERT(value == sent);
	result = raw_give(raw3, &value, SIZE);        ASSERT_success(result);
	result = raw_wait(raw2, &value, SIZE, &read); ASSERT_success(result);
	                                              ASSERT(read == SIZE);
	                                              ASSERT(value == sent);
	result = raw_give(raw1, &value, SIZE);        ASSERT_success(result);
	result = tsk_join(tsk3);                      ASSERT_success(result);
	         tsk_stop();
}

static void proc1()
{
	int result;
	unsigned value;
	unsigned read = SIZE;
	                                              ASSERT_dead(tsk2);
	         tsk_startFrom(tsk2, proc2);          ASSERT_ready(tsk2);
	result = raw_wait(raw1, &value, SIZE, &read); ASSERT_success(result);
	                                              ASSERT(read == SIZE);
	                                              ASSERT(value == sent);
	result = raw_give(raw2, &value, SIZE);        ASSERT_success(result);
	result = raw_wait(raw1, &value, SIZE, &read); ASSERT_success(result);
	                                              ASSERT(read == SIZE);
	                                              ASSERT(value == sent);
	result = raw_give(&raw0, &value, SIZE);       ASSERT_success(result);
	result = tsk_join(tsk2);                      ASSERT_success(result);
	         tsk_stop();
}

static void proc0()
{
	int result;
	unsigned value;
	unsigned read = SIZE;
	                                              ASSERT_dead(tsk1);
	         tsk_startFrom(tsk1, proc1);          ASSERT_ready(tsk1);
	result = raw_wait(&raw0, &value, SIZE, &read);ASSERT_success(result);
	                                              ASSERT(read == SIZE);
	                                              ASSERT(value == sent);
	result = raw_give(raw1, &value, SIZE);        ASSERT_success(result);
	result = raw_wait(&raw0, &value, SIZE, &read);ASSERT_success(result);
	                                              ASSERT(read == SIZE);
	                                              ASSERT(value == sent);
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
	         sent = (unsigned)rand();
	result = raw_give(&raw0, &sent, SIZE);        ASSERT_success(result);
	result = tsk_join(&tsk0);                     ASSERT_success(result);
}

extern "C"
void test_raw_buffer_2()
{
	TEST_Notify();
	TEST_Call();
}
