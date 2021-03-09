#include <stm32f4_discovery.h>
#include <os.h>

using namespace device;
using namespace stateos;

void consumer( Semaphore &sem )
{
	Led led;

	for (;;)
	{
		sem.wait();
		led.tick();
	}
}

void producer( Semaphore &sem )
{
	for (;;)
	{
		ThisTask::sleepFor(SEC);
		sem.give();
	}
}

int main()
{
	auto sem = Semaphore::Binary();

	auto cons = Task(0, consumer, std::ref(sem));
	auto prod = Task(0, producer, std::ref(sem));

	cons.start();
	prod.start();

	ThisTask::sleep();
}
