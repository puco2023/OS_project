#include "../h/MemoryAllocator.hpp"
#include "../h/RiscV.hpp"
#include "../h/TCB.hpp"
#include "../h/syscall_cpp.hpp"
#include "../h/KernelConsole.hpp"
#include "../lib/hw.h"

extern void userMain();
static void idle(void* arg) {
    while (true) {
        Thread::dispatch();
    }
}

static void joinWorker(void* arg) {
    (void)arg;
    for (int i = 0; i < 3; i++) {
        Console::putc('W');
        Thread::dispatch();
    }

    Console::putc('!');
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

    idleThread->start();

    Thread* worker = new Thread(joinWorker, nullptr);
    worker->start();

    worker->join(worker, 0);
    Console::putc('J');
    Console::putc('\n');

    delete worker;
    while (true)
        thread_dispatch();
    return 0;
}
