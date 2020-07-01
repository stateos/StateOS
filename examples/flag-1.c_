#include <stm32f4_discovery.h>
#include <os.h>

OS_FLG(flg, 0);

void consumer()
{
	flg_wait(flg, 3, flgAll);
	LED_Tick();
}

void producer()
{
	tsk_delay(SEC/2);
	flg_give(flg, 1);
	tsk_delay(SEC/2);
	flg_give(flg, 2);
}

OS_TSK(cons, 0, consumer);
OS_TSK(prod, 0, producer);

int main()
{
	LED_Init();

	tsk_start(cons);
	tsk_start(prod);
	tsk_stop();
}
