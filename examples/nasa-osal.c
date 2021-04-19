#include <stm32f4_discovery.h>
#include <osapi.h>

uint32 sem;
uint32 cons, prod;

void consumer(void)
{
	OS_TaskRegister();

	for (;;)
	{
		OS_BinSemTake(sem);
		LED_Tick();
	}
}

void producer(void)
{
	OS_TaskRegister();

	for (;;)
	{
		OS_TaskDelay(1000);
		OS_BinSemGive(sem);
	}
}

int main()
{
	LED_Init();
	OS_API_Init();
	OS_BinSemCreate(&sem, "sem", 0, 0);
	OS_TaskCreate(&cons, "cons", consumer, 0, 0, 255, 0);
	OS_TaskCreate(&prod, "prod", producer, 0, 0, 255, 0);
	OS_TaskExit();
}
