#include <stm32f4_discovery.h>
#include <osapi.h>

void timer(osal_id_t id)
{
	(void) id;

	LED_Tick();
}

int main()
{
	osal_id_t tmr;
	uint32 accuracy;

	LED_Init();
	OS_Application_Startup();
	OS_TimerCreate(&tmr, "tmr", &accuracy, timer);
	OS_TimerSet(tmr, 1000000, 1000000);
	OS_Application_Run();
	OS_TaskExit();
}
