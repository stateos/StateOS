#include "test.h"

static_SIG(sig3);

static unsigned sent;

static void proc3()
{
	unsigned value;
	int result;

	result = sig_wait(sig3, sigAll, &value);      ASSERT_success(result);
	                                              ASSERT(value == sent);
	         sig_give(sig2, value);
	         tsk_stop();
}

static void proc2()
{
	unsigned value;
	int result;
	                                              ASSERT_dead(tsk3);
	         tsk_startFrom(tsk3, proc3);          ASSERT_ready(tsk3);
	result = sig_wait(sig2, sigAll, &value);      ASSERT_success(result);
	                                              ASSERT(value == sent);
	         sig_give(sig3, value);
	result = sig_wait(sig2, sigAll, &value);      ASSERT_success(result);
	                                              ASSERT(value == sent);
	         sig_give(sig1, value);
	result = tsk_join(tsk3);                      ASSERT_success(result);
	         tsk_stop();
}

static void proc1()
{
	unsigned value;
	int result;
	                                              ASSERT_dead(tsk2);
	         tsk_startFrom(tsk2, proc2);          ASSERT_ready(tsk2);
	result = sig_wait(sig1, sigAll, &value);      ASSERT_success(result);
	                                              ASSERT(value == sent);
	         sig_give(sig2, value);
	result = sig_wait(sig1, sigAll, &value);      ASSERT_success(result);
	                                              ASSERT(value == sent);
	         sig_give(&sig0, value);
	result = tsk_join(tsk2);                      ASSERT_success(result);
	         tsk_stop();
}

static void proc0()
{
	unsigned value;
	int result;
	                                              ASSERT_dead(tsk1);
	         tsk_startFrom(tsk1, proc1);          ASSERT_ready(tsk1);
	result = sig_wait(&sig0, sigAll, &value);     ASSERT_success(result);
	                                              ASSERT(value == sent);
	         sig_give(sig1, value);
	result = sig_wait(&sig0, sigAll, &value);     ASSERT_success(result);
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
	         sent = (unsigned)rand() % SIG_LIMIT;
	         sig_give(&sig0, sent);
	result = tsk_join(&tsk0);                     ASSERT_success(result);
}

void test_signal_1()
{
	TEST_Notify();
	TEST_Call();
}
