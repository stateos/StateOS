#include <stm32f4_discovery.h>
#include <os.h>

OS_MSG(msg, 1, sizeof(unsigned));

OS_TSK_DEF(cons, 0)
{
	for (;;)
	{
		msg_wait(msg, NULL, 0, NULL);
		LED_Tick();
	}
}

OS_TSK_DEF(prod, 0)
{
	for (;;)
	{
		tsk_delay(SEC);
		msg_give(msg, NULL, 0);
	}
}

int main()
{
	LED_Init();

	tsk_start(cons);
	tsk_start(prod);
	tsk_sleep();
}
