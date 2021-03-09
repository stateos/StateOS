#include <stm32f4_discovery.h>
#include <os.h>

using namespace device;
using namespace stateos;

auto led = Led();
auto stm = StreamBufferTT<1, unsigned>();

void consumer()
{
	unsigned x;

	for (;;)
	{
		stm.wait(&x);
		led = x;
	}
}

void producer()
{
	unsigned x = 1;

	for (;;)
	{
		ThisTask::delay(SEC);
		stm.send(&x);
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
