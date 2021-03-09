#include <stm32f4_discovery.h>
#include <os.h>

using namespace device;
using namespace stateos;

auto led = Led();
auto lst = ListTT<unsigned>();
auto mem = MemoryPoolTT<1, unsigned>();

void consumer()
{
	unsigned*p;

	for (;;)
	{
		lst.wait(&p);
		led = *p;
		mem.give(p);
	}
}

void producer()
{
	unsigned*p;
	unsigned x = 1;

	for (;;)
	{
		ThisTask::delay(SEC);

		mem.wait(&p);
		*p=x;
		lst.give(p);
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
