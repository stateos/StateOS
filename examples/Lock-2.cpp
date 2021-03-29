#include <stm32f4_discovery.h>
#include <os.h>

using namespace device;
using namespace stateos;

auto led = Led();
auto mtx = Mutex(mtxDefault);

void consumer()
{
	auto lock = Lock<Mutex>(mtx);
	led.tick();
}

void producer()
{
	auto lock = Lock<Mutex>(mtx);
	thisTask::sleepFor(SEC);
}

auto prod = Task::Start(2, producer);
auto cons = Task::Start(1, consumer);

int main()
{
	thisTask::sleep();
}
