#include "test.h"

static_SIG(sig3, 0);

static unsigned sent;

static void proc3()
{
	unsigned event;

	event = sig_wait(sig3, sigAll);              ASSERT(event == sent);
 	        sig_give(sig2, event);
	        tsk_stop();
}

static void proc2()
{
	unsigned event;
		                                         ASSERT_dead(tsk3);
	        tsk_startFrom(tsk3, proc3);          ASSERT_ready(tsk3);
	event = sig_wait(sig2, sigAll);              ASSERT(event == sent);
 	        sig_give(sig3, event);
	event = sig_wait(sig2, sigAll);              ASSERT(event == sent);
 	        sig_give(sig1, event);
	event = tsk_join(tsk3);                      ASSERT_success(event);
	        tsk_stop();
}

static void proc1()
{
	unsigned event;
		                                         ASSERT_dead(tsk2);
	        tsk_startFrom(tsk2, proc2);          ASSERT_ready(tsk2);
	event = sig_wait(sig1, sigAll);              ASSERT(event == sent);
 	        sig_give(sig2, event);
	event = sig_wait(sig1, sigAll);              ASSERT(event == sent);
 	        sig_give(&sig0, event);
	event = tsk_join(tsk2);                      ASSERT_success(event);
	        tsk_stop();
}

static void proc0()
{
	unsigned event;
		                                         ASSERT_dead(tsk1);
	        tsk_startFrom(tsk1, proc1);          ASSERT_ready(tsk1);
	event = sig_wait(&sig0, sigAll);             ASSERT(event == sent);
 	        sig_give(sig1, event);
	event = sig_wait(&sig0, sigAll);             ASSERT(event == sent);
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
 	        sig_give(&sig0, sent = rand() % SIG_LIMIT);
	event = tsk_join(&tsk0);                     ASSERT_success(event);
}

void test_signal_1()
{
	TEST_Notify();
	TEST_Call();
}
