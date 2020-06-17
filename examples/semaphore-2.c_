#include <stm32f4_discovery.h>
#include <os.h>

sem_t sem = SEM_INIT(0, semBinary);

void consumer()
{
	sem_wait(&sem);
	LED_Tick();
}

void producer()
{
	tsk_delay(SEC);
	sem_give(&sem);
}

tsk_t cons = TSK_INIT(0, consumer);
tsk_t prod = TSK_INIT(0, producer);

int main()
{
	LED_Init();

	tsk_start(&cons);
	tsk_start(&prod);
	tsk_stop();
}
