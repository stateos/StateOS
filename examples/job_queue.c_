#include <stm32f4_discovery.h>
#include <os.h>

OS_JOB(job, 1);

void tick()
{
	LED_Tick();
}

OS_TSK_START(cons, 0)
{
	job_wait(job);
}

OS_TMR_START(tmr, SEC, SEC)
{
	job_giveISR(job, tick);
}

int main()
{
	LED_Init();
	tsk_sleep();
}
