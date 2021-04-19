#include <stm32f4_discovery.h>
#include <cmsis_os2.h>

osSemaphoreId_t sem;

void consumer(void *arg)
{
	(void) arg;

	for (;;)
	{
		osSemaphoreAcquire(sem, osWaitForever);
		LED_Tick();
	}
}

void producer(void *arg)
{
	(void) arg;

	for (;;)
	{
		osDelay(1000);
		osSemaphoreRelease(sem);
	}
}

int main(void)
{
	LED_Init();
	osKernelInitialize();
	sem = osSemaphoreNew(1, 0, NULL);
	osThreadNew(consumer, NULL, NULL);
	osThreadNew(producer, NULL, NULL);
	osKernelStart();
	osThreadExit();
}
