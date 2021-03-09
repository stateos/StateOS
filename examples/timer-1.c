#include <stm32f4_discovery.h>
#include <os.h>

OS_SEM(sem, 0, semBinary);

void consumer()
{
	sem_wait(sem);
	LED_Tick();
}

void proc()
{
	sem_giveISR(sem);
}

OS_TSK(cons, 0, consumer);
OS_TMR(tmr, proc);

int main()
{
	LED_Init();

	tsk_start(cons);
	tmr_startPeriodic(tmr, SEC);
	tsk_stop();
}
