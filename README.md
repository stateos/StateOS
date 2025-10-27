# StateOS
  [![](https://img.shields.io/github/license/stateos/StateOS.svg?style=flat-square&logo)](https://opensource.org/licenses/MIT)
  [![](https://img.shields.io/github/release/stateos/StateOS.svg?style=flat-square&logo)](https://github.com/stateos/StateOS/releases)
  [![](https://github.com/stateos/StateOS/actions/workflows/test.yml/badge.svg)](https://github.com/stateos/StateOS/actions/workflows/test.yml)

StateOS is free, extremely simple and very fast real-time operating system (RTOS) designed for deeply embedded applications.

### Targets

ARM Cortex-M, STM8.

### Inspiration

StateOS was inspired by the concept of a state machine. Procedure executed by the task (task state) doesn't have to be noreturn-type. It will be executed into an infinite loop. There's a dedicated function for immediate change the task state (task function).

### Documentation

All documentation is contained within source files, in particular header files.

### Gettin started

Building an application for a specific compiler is realised using the appropriate makefile script.

### Features

- kernel can operate in preemptive or cooperative mode
- kernel can operate with 16, 32 or 64-bit timer counter
- kernel can operate in tick-less mode
- implemented basic protection using MPU (use nullptr, stack overflow)
- implemented functions for asynchronous communication with unmasked interrupt handlers
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
- read/write locks
- memory pools
- raw buffers
- message queues
- mailbox queues
- event queues
- job queues
- timers (one-shot, periodic)
- hierarchical state machine
- cmsis-rtos api
- cmsis-rtos2 api
- c++ wrapper
- support for std::thread
- all documentation is contained within source files, in particular header files
- examples and templates are in separate repositories on [GitHub](https://github.com/stateos)
- archival releases on [sourceforge](https://sourceforge.net/projects/stateos)

### Supported stdc++ features

- std::thread, std::jthread, std::stop_callback, std::this_thread, ...
- std::mutex, std::shared_mutex, std::condition_variable, ...
- std::binary_semaphore, std::counting_semaphore, ...
- std::lock_guard, std::unique_lock, std::shared_lock, ...
- std::future, std::promise, std::assync, ...
- std::barrier, std::latch, ...
- std::chrono, ...
- thread local storage (thread_local)
- and more ...

### License

This project is licensed under the terms of the [MIT License (MIT)](https://opensource.org/licenses/MIT).
