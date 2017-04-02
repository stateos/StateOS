#include <stm32f4_discovery.h>
#include <os.h>

OS_TMR_DEF(tmr)
{
	LED_Tick();
}

int main()
{
	LED_Init();

	tmr_startPeriodic(tmr, SEC);
	tsk_stop();
}
