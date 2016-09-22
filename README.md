StateOS
=======

Free, extremely simple and amazingly tiny real-time operating system (RTOS) designed for deeply embedded applications.
Target: ARM Cortex-M family.
It was inspired by the concept of a state machine.
Procedure executed by the task (task state) doesn't have to be noreturn-type.
It will be executed into an infinite loop.
There's a dedicated function for immediate change the task state.

Features
--------

- kernel works in preemptive or cooperative mode
- kernel can operate in tick-less mode (32-bit timer required)
- signals (auto clearing, protected)
- events
- flags (one, all, accept, ignore)
- barriers
- semaphores (binary, limited, counting)
- mutexes (recursive and priority inheritance)
- fast mutexes (non-recursive and non-priority-inheritance)
- condition variables
- message queues
- mailbox queues
- timers (one-shot, periodic)
- c++ wrapper
- all documentation is contained within the source files
- examples and templates are in separate repositories on [GitHub](https://github.com/stateos)

Targets
-------

ARM CM0(+), CM3, CM4(F), CM7
