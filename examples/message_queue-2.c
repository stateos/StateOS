#include <stm32f4_discovery.h>
#include <os.h>

OS_MSG(msg, 1, sizeof(unsigned));

OS_TSK_DEF(cons, 0)
{
	unsigned dummy;

	for (;;)
	{
		msg_wait(msg, &dummy, sizeof(dummy), NULL);
		LED_Tick();
	}
}

OS_TSK_DEF(prod, 0)
{
	unsigned dummy;

	for (;;)
	{
		tsk_delay(SEC);
		msg_give(msg, &dummy, sizeof(dummy));
	}
}

int main()
{
	LED_Init();

	tsk_start(cons);
	tsk_start(prod);
	tsk_sleep();
}
