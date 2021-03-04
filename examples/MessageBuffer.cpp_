#include <stm32f4_discovery.h>
#include <os.h>

using namespace device;
using namespace stateos;

auto led = Led();
auto msg = MessageBufferTT<1, unsigned>();

void consumer()
{
	unsigned x;

	for (;;)
	{
		msg.wait(&x);
		led = x;
	}
}

void producer()
{
	unsigned x = 1;

	for (;;)
	{
		ThisTask::delay(SEC);
		msg.give(&x);
		x = (x << 1) | (x >> 3);
	}
}

auto cons = Task(0, consumer);
auto prod = Task(0, producer);

int main()
{
	cons.start();
	prod.start();

	ThisTask::stop();
}
