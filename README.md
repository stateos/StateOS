# StateOS
  [![Build Status](https://travis-ci.org/stateos/StateOS.svg)](https://travis-ci.org/stateos/StateOS)
  [![Software License](https://img.shields.io/github/license/stateos/StateOS.svg)](https://opensource.org/licenses/MIT)
  [![Github Release](https://img.shields.io/github/release/stateos/StateOS.svg)](https://github.com/stateos/StateOS/releases)

Free, extremely simple, amazingly tiny and very fast real-time operating system (RTOS) designed for deeply embedded applications.
Target: ARM Cortex-M, STM8 families.
It was inspired by the concept of a state machine.
Procedure executed by the task (task state) doesn't have to be noreturn-type.
It will be executed into an infinite loop.
There's a dedicated function for immediate change the task state.

### Features

- kernel works in preemptive or cooperative mode
- kernel can operate with 16, 32 or 64-bit timer counter
- kernel can operate in tick-less mode
- signals (clear, protect)
- events
- flags (any, all, protect, ignore)
- barriers
- semaphores (binary, limited, counting)
- mutexes (recursive, priority inheritance, robust)
- fast mutexes (non-recursive, non-priority-inheritance, non-robust)
- condition variables
- memory pools
- message queues
- mailbox queues
- job queues
- timers (one-shot, periodic)
- cmsis-rtos api
- cmsis-rtos2 api
- nasa-osal support
- c++ wrapper
- all documentation is contained within the source files
- examples and templates are in separate repositories on [GitHub](https://github.com/stateos)
- archival releases on [sourceforge](https://sourceforge.net/projects/stateos)

### Targets

ARM CM0(+), CM3, CM4(F), CM7

### License

This project is licensed under the terms of [the MIT License (MIT)](https://opensource.org/licenses/MIT).
