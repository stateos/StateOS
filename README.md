StateOS
=======

Free real-time operating system (RTOS) designed for deeply embedded applications.
It was inspired by the concept of a state machine.

Features
--------

- kernel works in preemptive or cooperative mode
- kernel can operate in tick-less mode
- events
- flags (one, all, clear, accept)
- barriers
- semaphores (direct, binary, limited, normal)
- mutexes (normal, recursive, priority protect, priority inheritance)
- condition variables
- message queues
- mail box queues
- timers (one-shot, periodic)
- c++ wrapper

Targets
-------

ARM CM0(+), CM3, CM4(F), CM7

(uC must have 32-bit timer counter for tick-less mode)

License
-------

This project is licensed under the terms of [GNU GENERAL PUBLIC LICENSE Version 3](http://www.gnu.org/philosophy/why-not-lgpl.html).
