#include <stm32f4_discovery.h>
#include <os.h>

using namespace device;
using namespace stateos;

Led       led;
Semaphore sem{0};

void consumer()
{
	for (;;)
	{
		sem.wait();
		led.tick();
	}
}

void producer()
{
	for (;;)
	{
		thisTask::sleepFor(SEC);
		sem.give();
	}
}

TaskT<256> cons{consumer};
TaskT<256> prod{producer};

int main()
{
	cons.start();
	prod.start();

	thisTask::stop();
}
