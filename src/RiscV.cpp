#include "../h/RiscV.hpp"
#include"../h/TCB.hpp"
#include "../lib/hw.h"
#include "../h/SyscallCodes.hpp"
#include "../h/MemoryAllocator.hpp"
#include"../h/Syscall_c_api.hpp"
#include "../h/KernelConsole.hpp"
void RiscV::handleSupervisorTrap() {
    uint64 code, arg1, arg2, arg3, arg4;
    __asm__ volatile(
        "mv %0, a0\n\t"
        "mv %1, a1\n\t"
        "mv %2, a2\n\t"
        "mv %3, a3\n\t"
        "mv %4, a4"
        : "=r"(code), "=r"(arg1), "=r"(arg2), "=r"(arg3), "=r"(arg4)
    );

    uint64 scause = r_scause();

    if (scause == 0x0000000000000009UL || scause == 0x0000000000000008UL) {
        uint64 sepc = r_sepc() + 4;
        volatile uint64 sstatus = r_sstatus();

        uint64 ret = 0;

        switch (code) {
            case MEM_ALLOC:
                ret = (uint64)MemoryAllocator::mem_alloc((size_t)arg1);
                break;

            case MEM_FREE:
                ret = (uint64)MemoryAllocator::mem_free((void*)arg1);
                break;

            case MEM_GET_FREE_SPACE:
                ret = MemoryAllocator::mem_get_free_space();
                break;

            case MEM_GET_LARGEST_FREE_BLOCK:
                ret = MemoryAllocator::mem_get_largest_free_block();
                break;

            case THREAD_DISPATCH: {
                TCB::dispatch();
                w_sstatus(sstatus);
                w_sepc(sepc);
                return;
            }

            case THREAD_CREATE: {
                ret = TCB::createThread(
                (TCB**)arg1,
                (TCB::Body)arg2,
                (void*)arg3,
    (void*)arg4);
                break;
            }
            case THREAD_EXIT: {
                TCB::running->thread_exit();
                w_sstatus(sstatus);
                w_sepc(sepc);
                return;
            }
            case SEM_OPEN: {
                sem_t* handle = (sem_t*)arg1;
                unsigned init = (unsigned)arg2;
                *handle = new _sem(init);
                ret = (*handle != nullptr) ? 0 : (uint64)-1;
                break;
            }
            case SEM_CLOSE: {
                sem_t id = (sem_t)arg1;
                ret = id ? (uint64)id->close() : (uint64)-1;
                break;
            }
            case SEM_WAIT: {
                sem_t id = (sem_t)arg1;
                ret = (uint64)id->wait();
                break;
            }
            case SEM_SIGNAL: {
                sem_t id = (sem_t)arg1;
                ret = (uint64)id->signal();
                break;
            }
            case SEM_WAIT_N: {
                sem_t id = (sem_t)arg1;
                unsigned n = (unsigned)arg2;

                ret = id ? id->wait_n(n) : -1;
                break;
            }

            case SEM_SIGNAL_N: {
                sem_t id = (sem_t)arg1;
                unsigned n = (unsigned)arg2;
                ret = id ? id->signal_n(n) : -1;
                break;
            }
            case TIME_SLEEP: {
                ret = (uint64)TCB::sleep((time_t)arg1);
                break;
            }
            case GETC: {
                char c = KernelConsole::getInstance()->getc();
                uint64 ret = (uint64) c;
                __asm__ volatile("mv a0, %0" : : "r"(ret));
                w_sstatus(sstatus);
                w_sepc(sepc);
                return;
            }

            case PUTC: {
                char c = (char)arg1;
                KernelConsole::getInstance()->putc(c);
                break;
            }
        }

        __asm__ volatile("mv a0, %0" : : "r"(ret));

        w_sstatus(sstatus);
        w_sepc(sepc);

        return;
    }

    if (scause == 2UL) {
        TCB::running->setFinished(true);
        TCB::timeSliceCounter = 0;
        TCB::dispatch();
        return;
    }

    if (scause == 0x8000000000000001UL) {
        uint64 sepc = r_sepc();
        uint64 sstatus = r_sstatus();

        static uint64 tickCount = 0;
        tickCount++;
        TCB::tickSleeping();
        TCB::timeSliceCounter++;
        mc_sip(SIP_SSIP);
        if (TCB::timeSliceCounter >= TCB::running->getTimeSlice()) {
            TCB::timeSliceCounter = 0;
            TCB::dispatch();
        }
        w_sstatus(sstatus);
        w_sepc(sepc);
    }
    if (scause == 0x8000000000000009UL) {
        int irq = plic_claim();

        if (irq == 10) {
            KernelConsole::getInstance()->console_handler();
        }

        plic_complete(irq);
    }
}
