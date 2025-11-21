#include <stm32f4_discovery.h>
#include <os.h>
#include <chrono>

enum
{
	EventALL    = hsmALL,
	EventStop   = hsmStop,
	EventExit   = hsmExit,
	EventEntry  = hsmEntry,
	EventInit   = hsmInit,
	EventSwitch = hsmUser,
	EventTick,
};

auto led        = device::Led();
auto dispatcher = os::Task(nullptr);
auto StateOff   = os::State();
auto StateOn    = os::State();
auto blinker    = os::StateMachineT<10>::Create
({
	{ StateOff, EventInit,   []( hsm_t *, unsigned ){ led = 0; } },
	{ StateOff, EventSwitch, StateOn },
});

int main()
{
	blinker->add(StateOn, EventSwitch, StateOff);
	blinker->add(StateOn, EventTick,   []( hsm_t *, unsigned ){ led.tick(); });

	blinker->start(dispatcher, StateOff);
	blinker->send(EventSwitch);
	for (;;)
	{
		os::thisTask::delay(std::chrono::seconds(1));
		blinker->send(EventTick);
	}
}
