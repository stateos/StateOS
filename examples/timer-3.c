#include <stm32f4_discovery.h>
#include <os.h>

OS_TMR_START(tmr, SEC, SEC)
{
	LED_Tick();
}

int main()
{
	LED_Init();
	tsk_sleep();
}
