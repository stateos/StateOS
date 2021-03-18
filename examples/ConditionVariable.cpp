#include <stm32f4_discovery.h>
#include <os.h>

using namespace device;
using namespace stateos;

auto led = Led();
auto mtx = Mutex(mtxDefault);
auto cnd = ConditionVariable();

void consumer()
{
	mtx.wait();
	for (;;)
	{
		cnd.wait(mtx);
		led.tick();
	}
}

void producer()
{
	mtx.wait();
	this_task::delay(SEC);
	cnd.give(cndOne);
	mtx.give();
}

auto cons = Task(1, consumer);
auto prod = Task(1, producer);

int main()
{
	cons.start();
	prod.start();

	this_task::stop();
}
