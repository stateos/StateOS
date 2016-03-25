#include <stm32f4_discovery.h>
#include <os.h>

auto led = Led();
auto sem = Semaphore(0);
auto sla = startTask(0, []{ sem.wait(); led.tick(); });
auto mas = startTask(0, []{ ThisTask::sleepFor(SEC); sem.give(); });

int main()
{
	ThisTask::stop();
}
