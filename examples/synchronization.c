#include <stm32f4_discovery.h>
#include <os.h>

void resetLed()
{
	LED_Init();
	LEDs = 0;
}

OS_ONE(one);

void tickLoop(sem_t *own, sem_t *another)
{
	for (;;)
	{
		one_call(one, resetLed);
		sem_wait(own);
		tsk_delay(SEC);
		LED_Tick();
		sem_post(another);
	}
}

OS_SEM(cons_sem, 0);
OS_SEM(prod_sem, 1);

OS_TSK_START(cons, 0) { tickLoop(cons_sem, prod_sem); }
OS_TSK_START(prod, 0) { tickLoop(prod_sem, cons_sem); }

int main()
{
	tsk_sleep();
}
