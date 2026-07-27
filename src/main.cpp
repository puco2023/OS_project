#include "../h/MemoryAllocator.hpp"
#include "../h/RiscV.hpp"
#include "../h/TCB.hpp"
#include "../h/syscall_cpp.hpp"
#include "../h/KernelConsole.hpp"
#include "../lib/hw.h"
<<<<<<< Updated upstream

extern void userMain();

_sem* printSem = nullptr;

=======
#include"../h/Printing.hpp"
>>>>>>> Stashed changes
static void idle(void* arg) {
    while (true) {
        Thread::dispatch();
    }
}
<<<<<<< Updated upstream
void printString(const char* string) {
    while (*string!='\0') {
        putc(*string);
        string++;
    }
}
void printInteger(uint64 integer)
{
    static char digits[] = "0123456789";
    char buf[16];
    int i, neg;
    uint x;

    neg = 0;
    if (integer < 0)
    {
        neg = 1;
        x = -integer;
    }
    else
    {
        x = integer;
    }

    i = 0;
    do
    {
        buf[i++] = digits[x % 10];
    } while ((x /= 10) != 0);

    if (neg)
        buf[i++] = '-';

    while (--i >= 0)
        putc(buf[i]);
}
class ThreadC: public Thread {
private:
    int id;
    public:
    ThreadC(int i):Thread(),id(i){}
    void run()override {
        printSem->wait();
        printString("cao ja sam nit broj: ");
        printInteger(id);
        printString("\n");
        printSem->signal();
        sleep(50);
=======
class ThreadA:public Thread{
public:
    int id;
    ThreadA(int id):Thread() {
        this->id = id;
    }
    void run()override {
        printString("cao ja sam nit broj");
        printInteger(this->id);
        printString("\n");
        sleep(10);
>>>>>>> Stashed changes
    }
};
int main() {
    RiscV::w_stvec((uint64) &RiscV::supervisorTrap);
    MemoryAllocator::init();
    printSem = new _sem(1);

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
<<<<<<< Updated upstream
    idleThread->start();
    Thread::setMaximumThreads(3,20,10);
    for (int i=0;i<20;i++) {
        ThreadC* tc = new ThreadC(i+1);
        tc->start();
    }
    while (true) {
        thread_dispatch();
    }
}
=======

    //Semaphore* sem = new Semaphore(0);
    idleThread->start();
    //Thread* userThread = new Thread(userMainWrapper, sem);
    //userThread->start();
    //sem->wait();

    //delete sem;
    //Thread::setMaximumThreads(3,20,10);
    ThreadA* ta[20];
    for (int i=0;i<20;i++) {
        ta[i] = new ThreadA(i);
    }
    Thread::setMaximumThreads(3,20,10);

    for (int i=0;i<20;i++) {
        ta[i]->start();
    }

    return 0;
}
>>>>>>> Stashed changes
