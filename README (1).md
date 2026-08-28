# RISC-V Educational Operating System Kernel

An educational operating system kernel written in C++ and RISC-V assembly. The project implements basic kernel services, including dynamic memory allocation, thread management, scheduling, semaphores, time-based blocking, system calls, privilege transitions, and asynchronous console input/output.

## Features

- Custom dynamic memory allocator
- Lightweight kernel threads
- Preemptive and cooperative context switching
- Thread creation, termination, dispatch, and synchronization
- Parent-child thread tracking and `joinAll` support
- Counting semaphores with thread blocking and unblocking
- Extended semaphore operations for requesting and releasing multiple units
- Sleeping threads and timer interrupt handling
- C and C++ system-call APIs
- User and supervisor execution modes
- RISC-V trap and interrupt handling
- Buffered console input and output
- Idle thread for periods without runnable user threads

## Architecture

The system is divided into several logical layers:

1. **User API** – C and C++ functions used by application code.
2. **System-call interface** – transfers requests to the kernel using the RISC-V `ecall` instruction.
3. **Trap handler** – reads the system-call code and arguments, processes exceptions and interrupts, and returns the result through register `a0`.
4. **Kernel services** – thread management, scheduling, semaphores, memory allocation, sleeping, and console communication.
5. **Hardware abstraction** – access to RISC-V control and status registers, timer interrupts, and the platform interrupt controller.

## Main Components

| Component | Responsibility |
| --- | --- |
| `MemoryAllocator` | Allocation and release of memory blocks |
| `TCB` | Thread state, context, stack, and thread relationships |
| `Scheduler` | Queue of runnable threads and thread selection |
| `_sem` | Kernel semaphore implementation and blocked-thread queue |
| `RiscV` | Trap handling, CSR access, and privilege-mode transitions |
| `KernelConsole` | Buffered console input and output |
| System-call API | Communication between user code and the kernel |

## System Calls

The kernel supports system calls from the following groups:

- Memory: allocation, deallocation, and free-space information
- Threads: create, exit, dispatch, join, and related synchronization
- Semaphores: open, close, wait, signal, and multi-unit operations
- Time: putting the current thread to sleep for a specified number of timer ticks
- Console: reading and writing characters

Arguments are passed through RISC-V argument registers (`a0`–`a4`). The system-call identifier is placed in `a0`, while a return value, when present, is also returned through `a0` after the `ecall` instruction completes.

## Thread States

A thread can be:

- ready to run;
- currently running;
- blocked on a semaphore;
- sleeping until a timer deadline;
- waiting for another thread or its child threads;
- finished.

The scheduler only selects runnable threads. When no user thread is ready, the idle thread keeps the processor active until new work becomes available.

## Trap and Interrupt Handling

The supervisor trap handler processes:

- system calls from user and supervisor mode;
- timer interrupts used for time slicing and sleeping threads;
- external interrupts used by the console;
- unexpected exceptions and error conditions.

Important RISC-V registers used by the kernel include `scause`, `sepc`, `sstatus`, `stvec`, `stval`, `sie`, and `sip`.

## Building and Running

The project requires a RISC-V cross-compiler, GNU Make, and a compatible RISC-V emulator such as QEMU.

```bash
make
```

Run the generated kernel using the run target provided by the project Makefile, commonly:

```bash
make qemu
```

To remove generated build files:

```bash
make clean
```

> The exact run target may differ depending on the supplied project environment. Check the `Makefile` if `make qemu` is not defined.

## Project Structure

A typical source layout is:

```text
.
├── h/       # Header files
├── src/     # C++, C, and assembly source files
├── lib/     # Platform-provided libraries and hardware definitions
├── test/    # Kernel and user-level tests
└── Makefile # Build configuration
```

## Testing

The implementation can be tested through user programs that exercise:

- allocation and deallocation of memory;
- creation and context switching of multiple threads;
- semaphore synchronization and blocking order;
- sleeping and awakening after timer ticks;
- parent-child thread synchronization;
- simultaneous console input and output.

## Educational Purpose

This project was developed to demonstrate core operating-system concepts on the RISC-V architecture. It focuses on understanding low-level execution, context switching, synchronization, interrupts, privilege levels, and communication between user programs and the kernel.

## Author

Developed as an academic operating-systems project.
