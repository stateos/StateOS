#include <stm32f4_discovery.h>
#include <os.h>

OS_SEM(sem, 0);

OS_TSK_START(cons, 0)
{
	sem_wait(sem);
	LED_Tick();
}

OS_TSK_START(prod, 0)
{
	tsk_delay(SEC);
	sem_give(sem);
}

int main()
{
	LED_Init();
	tsk_sleep();
}
