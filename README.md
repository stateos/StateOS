# StateOS
  [![Build Status](https://travis-ci.org/stateos/StateOS.svg)](https://travis-ci.org/stateos/StateOS)
  [![Software License](https://img.shields.io/github/license/stateos/StateOS.svg)](https://opensource.org/licenses/MIT)
  [![Github Release](https://img.shields.io/github/release/stateos/StateOS.svg)](https://github.com/stateos/StateOS/releases)
  [![Software Download](https://img.shields.io/sourceforge/dt/stateos.svg)](https://sourceforge.net/projects/stateos/files/latest/download)

  [![GitHub Release](https://img.shields.io/github/release/stateos/StateOS.svg)](https://github.com/stateos/StateOS/releases/latest)
  [![Github Releases](https://img.shields.io/github/downloads/stateos/StateOS/latest/total.svg)](https://github.com/stateos/StateOS/releases/latest)
  [![Github All Releases](https://img.shields.io/github/downloads/stateos/StateOS/total.svg)](https://github.com/stateos/StateOS/releases/latest)


Free, extremely simple, amazingly tiny and very fast real-time operating system (RTOS) designed for deeply embedded applications.
Target: ARM Cortex-M, STM8.
It was inspired by the concept of a state machine.
Procedure executed by the task (task state) doesn't have to be noreturn-type.
It will be executed into an infinite loop.
There's a dedicated function for immediate change the task state.

### Features

- kernel can operate in preemptive or cooperative mode
- kernel can operate with 16, 32 or 64-bit timer counter
- kernel can operate in tick-less mode
- spin locks
- once flags
- events
- signals with protection mask
- flags (any, all, protect, ignore)
- barriers
- semaphores (binary, limited, counting)
- mutexes with configurable type, protocol and robustness
- fast mutexes (error checking)
- condition variables
- memory pools
- stream buffers
- message buffers
- mailbox queues
- event queues
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
