#include <stm32f4_discovery.h>
#include <os.h>

OS_SEM(sem, 0, semNormal);

OS_TSK_DEF(sla, 0)
{
	sem_wait(sem);
	LED_Tick();
}

OS_TSK_DEF(mas, 0)
{
	tsk_delay(SEC);
	sem_give(sem);
}

int main()
{
	LED_Init();

	tsk_start(sla);
	tsk_start(mas);
	tsk_sleep();
}
