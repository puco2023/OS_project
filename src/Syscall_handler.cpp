#include "../lib/hw.h"
#include "../h/SyscallCodes.hpp"
#include "../h/MemoryAllocator.hpp"
#include "../h/TCB.hpp"
#include "../h/Semaphore.hpp"
extern "C" uint64 handleSupervisorTrap(uint64 syscallCode, uint64 arg1)
{
    uint64 scause;
    uint64 sepc;
    __asm__ volatile("csrr %0, scause": "=r"(scause));
    __asm__ volatile("csrr %0, sepc": "=r"(sepc));
    if(scause==0x08 || scause == 0x09)
    {
        sepc+=4;
        __asm__ volatile("csrw sepc, %0": :"r"(sepc));



        switch(syscallCode) {
            case MEM_ALLOC: {
                return (uint64)MemoryAllocator::mem_alloc((size_t)arg1);
            }
            case MEM_FREE: {
                return (uint64)MemoryAllocator::mem_free((void*)arg1);
            }
            case MEM_GET_FREE_SPACE:
            {
                size_t ret = MemoryAllocator::mem_get_free_space();
                __asm__ volatile ("mv a0, %0" : : "r"(ret));
                break;
            }

            case MEM_GET_LARGEST_FREE_BLOCK:
            {
                size_t ret = MemoryAllocator::mem_get_largest_free_block();
                __asm__ volatile ("mv a0, %0" : : "r"(ret));
                break;
            }
            case THREAD_DISPATCH: {
                TCB::dispatch();
                break;
            }
            case THREAD_CREATE: {
                uint64 a1, a2, a3, a4;

                __asm__ volatile("mv %0, a1" : "=r"(a1));
                __asm__ volatile("mv %0, a2" : "=r"(a2));
                __asm__ volatile("mv %0, a3" : "=r"(a3));
                __asm__ volatile("mv %0, a4" : "=r"(a4));

                TCB** handle = (TCB**) a1;
                TCB::Body body = (TCB::Body) a2;
                void* arg = (void*) a3;
                void* stack_space = (void*) a4;

                int ret = TCB::createThread(handle, body, arg, stack_space);

                __asm__ volatile("mv a0, %0" : : "r"((uint64)ret));

                break;
            }


        }

    }
    return 0;
}