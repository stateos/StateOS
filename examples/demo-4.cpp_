#include <stm32f4_discovery.h>
#include <os.h>

using namespace device;
using namespace stateos;
using namespace std::chrono_literals;

void proc( unsigned &led, Clock::duration start )
{
	ThisTask::sleepUntil(Clock::time_point(start));
	for (;;)
	{
		ThisTask::sleepNext(500ms);
		led++;
	}
}

int main()
{
	auto led = Led();
	auto grn = GreenLed();

	Task::Create(0, [&]{ proc(led[0],   0ms); });
	Task::Create(0, [&]{ proc(led[1], 125ms); });
	Task::Create(0, [&]{ proc(led[2], 250ms); });
	Task::Create(0, [&]{ proc(led[3], 375ms); });
	Task::Create(0, [&]{ proc(grn,    500ms); });

	ThisTask::stop();
}
