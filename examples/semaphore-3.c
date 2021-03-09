#include <stm32f4_discovery.h>
#include <os.h>

sem_id sem = SEM_CREATE(0, semBinary);

void consumer()
{
	sem_wait(sem);
	LED_Tick();
}

void producer()
{
	tsk_delay(SEC);
	sem_give(sem);
}

tsk_id cons = TSK_CREATE(0, consumer);
tsk_id prod = TSK_CREATE(0, producer);

int main()
{
	LED_Init();

	tsk_start(cons);
	tsk_start(prod);
	tsk_stop();
}
