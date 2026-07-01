#include "../h/MemoryAllocator.hpp"
#include "../h/RiscV.hpp"
#include "../h/TCB.hpp"
#include "../h/syscall_cpp.hpp"
#include "../h/KernelConsole.hpp"
#include "../lib/hw.h"
#include "../h/Printing.hpp"

void idle(void* arg) {
    while (true) {
        Thread::dispatch();
    }
}

class WorkerA : public Thread {
protected:
    void run() override {
        sync();
        printString("radi A");
    }
};

class WorkerB : public Thread {
protected:
    void run() override {
        sync();
        printString("radi B");
    }
};
int main() {
    MemoryAllocator::init();

    RiscV::w_stvec((uint64)&RiscV::supervisorTrap);

    TCB::running = TCB::createMainThread();

    KernelConsole::getInstance();

    Thread* idleThread = new Thread(idle, nullptr);
    Thread* getcThread = new Thread(KernelConsole::getc_handler_wrapper, nullptr);
    Thread* putcThread = new Thread(KernelConsole::putc_handler_wrapper, nullptr);

    idleThread->start();
    getcThread->start();
    putcThread->start();

    Thread* t1 = new WorkerA();
    Thread* t2 = new WorkerB();
    t1->start();
    t2->start();
    Thread::pair(t1,t2);

    RiscV::ms_sie(RiscV::SIE_SSIE | RiscV::SIE_SEIE);
    RiscV::ms_sstatus(RiscV::SSTATUS_SIE);

    while (true) {
        Thread::dispatch();
    }
}