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
		thisTask::sleepFor(SEC);
		sem.give();
	}
}

int main()
{
	auto sem = Semaphore::Binary();

	auto cons = Task(consumer, std::ref(sem));
	auto prod = Task(producer, std::ref(sem));

	cons.start();
	prod.start();

	thisTask::sleep();
}
