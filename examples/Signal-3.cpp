#include <stm32f4_discovery.h>
#include <os.h>

using namespace device;
using namespace stateos;

auto led = Led();
auto sig = Signal();

void consumer()
{
	unsigned x;

	for (;;)
	{
		sig.wait(sigAny, &x);
		led = x;
	}
}

void producer()
{
	unsigned x = 1;

	for (;;)
	{
		ThisTask::sleepFor(SEC);
		sig.give(x);
		x = (x << 1) | (x >> 3);
	}
}

int main()
{
	Task::Detached(0, consumer);
	Task::Detached(0, producer);

	ThisTask::sleep();
}
