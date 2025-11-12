#include "stm32f4_discovery.h"
#include <chrono>
#include "os.h"

using namespace std::chrono_literals;
using namespace device;
using namespace stateos;

auto led  = Led();
auto cons = Task::Start([]{ thisTask::sleepFor(50ms); led.tick(); });
auto prod = Task::Start([]{ thisTask::sleepNext(1s); suspend(); thisTask::sleepNext(1s); resume(); });

int main()
{
	thisTask::suspend();
}
