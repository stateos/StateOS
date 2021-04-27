#include <stm32f4_discovery.h>
#include <osapi.h>

uint32 sem;
uint32 cons, prod;

void consumer(void)
{
	for (;;)
	{
		OS_BinSemTake(sem);
		LED_Tick();
	}
}

void producer(void)
{
	for (;;)
	{
		OS_TaskDelay(1000);
		OS_BinSemGive(sem);
	}
}

int main()
{
	LED_Init();
	OS_Application_Startup();
	OS_BinSemCreate(&sem, "sem", 0, 0);
	OS_TaskCreate(&cons, "cons", consumer, NULL, 256, 1, 0);
	OS_TaskCreate(&prod, "prod", producer, NULL, 256, 2, 0);
	OS_Application_Run();
	OS_TaskExit();
}
