#include "../h/MemoryAllocator.hpp"
#include "../h/RiscV.hpp"
#include "../h/TCB.hpp"
#include "../h/syscall_cpp.hpp"
#include "../h/KernelConsole.hpp"
#include "../lib/hw.h"

extern void userMain();

static void userMainWrapper(void* arg) {
    Semaphore* sem = (Semaphore*) arg;
    userMain();
    sem->signal();
}

static void idle(void* arg) {
    while (true) {
        Thread::dispatch();
    }
}

int main() {
    RiscV::w_stvec((uint64) &RiscV::supervisorTrap);
    MemoryAllocator::init();

    TCB::running = TCB::createMainThread();

    KernelConsole::getInstance();

    RiscV::ms_sie(RiscV::SIE_SSIE | RiscV::SIE_SEIE);
    RiscV::ms_sstatus(RiscV::SSTATUS_SIE);

    Thread* idleThread = new Thread(idle, nullptr);

    Thread* putcHandlerThread =
        new Thread(KernelConsole::putc_handler_wrapper, nullptr);
    putcHandlerThread->start();

    Thread* getcHandlerThread =
        new Thread(KernelConsole::getc_handler_wrapper, nullptr);

    getcHandlerThread->start();

    Semaphore* sem = new Semaphore(0);
    idleThread->start();
    Thread* userThread = new Thread(userMainWrapper, sem);
    userThread->start();
    sem->wait();

    delete sem;

    return 0;
}