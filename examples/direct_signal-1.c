#include <stm32f4_discovery.h>
#include <os.h>

void action( unsigned signo )
{
	(void) signo;

	LED_Tick();
}

void consumer()
{
	tsk_action(SELF, action);
	for (;;)
		tsk_yield();
}

OS_TSK(cons, 0, consumer);

void producer()
{
	tsk_delay(SEC);
	tsk_give(cons, 1);
}

OS_TSK(prod, 0, producer);

int main()
{
	LED_Init();

	tsk_start(cons);
	tsk_start(prod);
	tsk_stop();
}
