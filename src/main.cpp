#include "../h/MemoryAllocator.hpp"
#include "../h/RiscV.hpp"
#include "../h/TCB.hpp"
#include "../h/syscall_cpp.hpp"
#include "../h/KernelConsole.hpp"
#include "../lib/hw.h"
#include "../h/TCB.hpp"
#include "../h/Printing.hpp"

static void idle(void* arg) {
    while (true) {
        Thread::dispatch();
    }
}
_sem* sem;
class ThreadPriority :public Thread {
public:
    ThreadPriority(int id,TCB::Priority p):Thread(p) {
        this->ID=id;
    }
    void run() override {
        for (int i=0;i<10;i++) {
            printString("cao ja sam nit");
            printInteger(ID);
            printString("\n");
            thread_dispatch();
        }
        sem->signal();
    }
private:
    int ID;

};
int main() {
    RiscV::w_stvec((uint64) &RiscV::supervisorTrap);
    MemoryAllocator::init();

    sem = new _sem(0);

    TCB::running = TCB::createMainThread();

    KernelConsole::getInstance();

    RiscV::ms_sie(RiscV::SIE_SSIE | RiscV::SIE_SEIE);
    RiscV::ms_sstatus(RiscV::SSTATUS_SIE);

    Thread* idleThread = new Thread(idle, nullptr, TCB::low);

    Thread* putcHandlerThread =
        new Thread(KernelConsole::putc_handler_wrapper, nullptr,TCB::low);
    putcHandlerThread->start();

    Thread* getcHandlerThread =
        new Thread(KernelConsole::getc_handler_wrapper, nullptr,TCB::low);

    getcHandlerThread->start();

    idleThread->start();
    ThreadPriority* tp1 = new ThreadPriority(1,TCB::low);
    ThreadPriority* tp2 = new ThreadPriority(2,TCB::low);
    ThreadPriority* tp3 = new ThreadPriority(3,TCB::low);

    ThreadPriority* tp4 = new ThreadPriority(4,TCB::medium);
    ThreadPriority* tp5 = new ThreadPriority(5,TCB::medium);
    ThreadPriority* tp6 = new ThreadPriority(6,TCB::medium);

    ThreadPriority* tp7 = new ThreadPriority(7,TCB::high);
    ThreadPriority* tp8 = new ThreadPriority(8,TCB::high);
    ThreadPriority* tp9 = new ThreadPriority(9,TCB::high);
    printString("low:\n");
    tp1->start();
    tp2->start();
    tp3->start();

    time_sleep(15);
    printString("medium:\n");
    tp4->start();
    tp5->start();
    tp6->start();

    time_sleep(15);
    printString("high:\n");
    tp7->start();
    tp8->start();
    tp9->start();
    time_sleep(15);
    sem->wait_n(9);
    return 0;
}