#include <stm32f4_discovery.h>
#include <os.h>

using namespace device;
using namespace stateos;

auto led = Led();
auto sig = Signal();

void consumer()
{
	for (;;)
	{
		sig.wait(SIGSET(1));
		led.tick();
	}
}

void producer()
{
	for (;;)
	{
		thisTask::sleepFor(SEC);
		sig.give(1);
	}
}

int main()
{
	auto cons = Task::Start(0, consumer);
	auto prod = Task::Start(0, producer);

	thisTask::sleep();
}
