#include <stm32f4_discovery.h>
#include <os.h>

OS_MTX(mtx, mtxNormal); // normal mutex works like a binary semaphore

void consumer()
{
	mtx_wait(mtx);
	LED_Tick();
}

void producer()
{
	tsk_delay(SEC);
	mtx_give(mtx);
}

OS_TSK(cons, 0, consumer, 256);
OS_TSK(prod, 0, producer, 256);

int main()
{
	LED_Init();

	tsk_start(cons);
	tsk_start(prod);
	tsk_stop();
}
