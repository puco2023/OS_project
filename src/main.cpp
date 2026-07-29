#include "../h/MemoryAllocator.hpp"
#include "../h/RiscV.hpp"
#include "../h/TCB.hpp"
#include "../h/syscall_cpp.hpp"
#include "../h/KernelConsole.hpp"
#include "../lib/hw.h"
<<<<<<< HEAD

extern void userMain();
=======
#include"../h/Printing.hpp"

>>>>>>> bolja verzija
static void idle(void* arg) {
    while (true) {
        Thread::dispatch();
    }
}
class ThreadA : public Thread {
public:
    void run() override {
        printString("cao ja sam prva nit\n");
        sleep(80);
    }
};
class ThreadB:public Thread {
public:
    void run() override {
        printString("cao ja sam druga nit\n");
        sleep(100);
    }
};

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

<<<<<<< HEAD
    Thread* worker = new Thread(joinWorker, nullptr);
    worker->start();

    worker->join(worker, 0);
    Console::putc('J');
    Console::putc('\n');

    delete worker;
    while (true)
        thread_dispatch();
=======
    ThreadA* ta = new ThreadA();
    Thread* tb = new ThreadB();

    ta->start();
    tb->start();
    ta->join(1);
    tb->join(10);
    printString("niti su prosle\n");
>>>>>>> bolja verzija
    return 0;
}
