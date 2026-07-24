#include "../h/MemoryAllocator.hpp"
#include "../h/RiscV.hpp"
#include "../h/TCB.hpp"
#include "../h/syscall_cpp.hpp"
#include "../h/KernelConsole.hpp"
#include "../lib/hw.h"
#include "../h/Printing.hpp"
extern void userMain();

static void idle(void* arg) {
    while (true) {
        Thread::dispatch();
    }
}

class ThreadC : public Thread {
protected:
    void run() override {
        printString("Nit C radi\n");
    }
};

class ThreadB : public Thread {
protected:
    void run() override {
        ThreadC* children[3];
        for (int i = 0; i < 3; i++) {
            children[i] = new ThreadC();
            children[i]->start();
            addChild(children[i]);
        }

        printString("Nit B napravila 3 niti C, ceka joinAll\n");
        joinAll();
        printString("Nit B - sva deca C su zavrsila\n");
    }
};

class ThreadA : public Thread {
protected:
    void run() override {
        ThreadB* bChildren[3];
        for (int i = 0; i < 3; i++) {
            bChildren[i] = new ThreadB();
            bChildren[i]->start();
            addChild(bChildren[i]);
        }

        ThreadC* cChild = new ThreadC();
        cChild->start();
        addChild(cChild);

        printString("Nit A napravila 3 niti B i 1 nit C, ceka joinAll\n");
        joinAll();
        printString("Nit A - sva deca su zavrsila\n");
    }
};

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

    ThreadA* a = new ThreadA();
    a->start();

    idleThread->start();


    return 0;
}