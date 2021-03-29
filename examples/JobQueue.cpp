#include <stm32f4_discovery.h>
#include <os.h>

using namespace device;
using namespace stateos;

auto led = Led();
auto job = JobQueueT<1>();
auto cons = Task::Start(0, []{ job.wait(); });
auto tmr = Timer::StartPeriodic(SEC, []{ job.give([]{ led.tick(); }); });

int main()
{
	thisTask::sleep();
}
