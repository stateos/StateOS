#include <stm32f4_discovery.h>
#include <os.h>
#include <vector>

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
auto dispatcher = stateos::Task(0, nullptr);
auto StateOff   = stateos::State();
auto StateOn    = stateos::State();
auto blinker    = stateos::StateMachineT<10>();

std::vector<stateos::Action> tab =
{
	{ StateOff, EventInit,   []( hsm_t *, unsigned ){ led = 0; } },
	{ StateOff, EventSwitch, StateOn },
	{ StateOn,  EventSwitch, StateOff },
	{ StateOn,  EventTick,   []( hsm_t *, unsigned ){ led.tick(); } },
};

int main()
{
	for (auto& a: tab) a.link();

	blinker.start(dispatcher, StateOff);
	blinker.send(EventSwitch);
	for (;;)
	{
		tsk_delay(SEC);
		blinker.send(EventTick);
	}
}
