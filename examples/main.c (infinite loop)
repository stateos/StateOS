#include <stm32f4_discovery.h>
#include <os.h>

int main()
{
	LED_Init();

	for (;;)
	{
		tsk_delay(SEC);
		LED_Tick();
	}
}
