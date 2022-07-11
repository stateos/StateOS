#include <stm32f4_discovery.h>
#include <os.h>

using namespace device;
using namespace stateos;

auto led = Led();
auto mtx = Mutex(mtxDefault);
auto cnd = ConditionVariable();

void consumer()
{
	for (;;)
	{
		auto lck = LockGuard(mtx);
		cnd.wait(mtx);
		led.tick();
	}
}

void producer()
{
	thisTask::delay(SEC);
	cnd.give(cndOne);
}

auto cons = Task(1, consumer);
auto prod = Task(1, producer);

int main()
{
	cons.start();
	prod.start();

	thisTask::stop();
}
