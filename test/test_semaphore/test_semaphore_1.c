#include "test.h"

static_SEM(sem3, 0, semBinary);

static void proc3()
{
	unsigned event;

 	event = sem_wait(sem3);                      assert_success(event);
 	event = sem_give(sem2);                      assert_success(event);
	        tsk_stop();
}

static void proc2()
{
	unsigned event;
		                                         assert_dead(tsk3);
	        tsk_startFrom(tsk3, proc3);
 	event = sem_wait(sem2);                      assert_success(event);
 	event = sem_give(sem3);                      assert_success(event);
 	event = sem_wait(sem2);                      assert_success(event);
 	event = sem_give(sem1);                      assert_success(event);
	event = tsk_join(tsk3);                      assert_success(event);
	        tsk_stop();
}

static void proc1()
{
	unsigned event;
		                                         assert_dead(tsk2);
	        tsk_startFrom(tsk2, proc2);
 	event = sem_wait(sem1);                      assert_success(event);
 	event = sem_give(sem2);                      assert_success(event);
 	event = sem_wait(sem1);                      assert_success(event);
 	event = sem_give(&sem0);                     assert_success(event);
	event = tsk_join(tsk2);                      assert_success(event);
	        tsk_stop();
}

static void proc0()
{
	unsigned event;
		                                         assert_dead(tsk1);
	        tsk_startFrom(tsk1, proc1);
 	event = sem_give(sem1);                      assert_success(event);
 	event = sem_wait(&sem0);                     assert_success(event);
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

void test_semaphore_1()
{
	TEST_Notify();
	TEST_Call();
}
