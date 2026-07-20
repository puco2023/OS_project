#include "../h/MemoryAllocator.hpp"
#include "../h/RiscV.hpp"
#include "../h/TCB.hpp"
#include "../h/syscall_cpp.hpp"
#include "../h/KernelConsole.hpp"
#include "../lib/hw.h"
static void idle(void* arg) {
    while (true) {
        Thread::dispatch();
    }
}
void printInteger(int integer) {
    static char digits[]="0123456789";
    bool neg=false;
    char buf[16];
    int x;
    if (integer<0) {
        neg=true;
        x=-integer;
    }
    else {
        x=integer;
    }
    int i=0;
    do {
        buf[i++] = digits[x%10];
    }
    while ((x/=10)!=0);
    if (neg) {
        buf[i++]='-';
    }
    while (--i>=0) {
        putc(buf[i]);
    }
}
Semaphore* mutex = nullptr;
void printString(const char* c) {
    while (*c!='\0') {
        putc(*c);
        c++;
    }
}
class ThreadA:public Thread {
private:
    int ID;
    Semaphore* sem;
public:
    ThreadA(int id,Semaphore* s) : Thread(){
        ID=id;
        sem=s;
    }
    void run() {
        for (int i=0;i<5;i++) {
            sem->wait();
            Thread::sleep(2);
            mutex->wait();
            printString("cao ja sam nit broj: ");
            printInteger(this->ID);

            printString(" moja iteracija je: ");
            printInteger(i);
            printString("\n");
            mutex->signal();
        }
    }
};
class ThreadB:public Thread {
public:
    ThreadB(int id,Semaphore* s) : Thread(){this->ID=id;sem=s;}
    void run() {
        int k=0;
        for (int i=0;i<25;i++) {
            k += sem->signal();
            Thread::sleep(2);
            //Thread::dispatch();
            mutex->wait();
            printString("ja sam glavna nit");
            printString(" moja iteracija je: ");
            printInteger(i);
            printString("\n");
            mutex->signal();
        }
        printString("konacan broj je: ");
        printInteger(k);
        printString("\n");
    }

private:
    int ID;
    Semaphore* sem;
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

    mutex = new Semaphore();
    Semaphore* s1=new Semaphore();
    Semaphore* s2=new Semaphore();
    Semaphore* s3=new Semaphore();
    Semaphore* s4=new Semaphore();
    Semaphore* s5=new Semaphore();
    Semaphore* s6=new Semaphore();
    Semaphore::pairSem(s1,s2);
    Semaphore::pairSem(s1,s3);
    Semaphore::pairSem(s1,s4);
    Semaphore::pairSem(s1,s5);
    Semaphore::pairSem(s1,s6);
    Thread* t1 = new ThreadA(1,s2);
    Thread* t2 = new ThreadA(2,s3);
    Thread* t3 = new ThreadA(3,s4);
    Thread* t4 = new ThreadA(4,s5);
    Thread* t5 = new ThreadA(5,s6);
    Thread* t6 = new ThreadB(9,s1);
    t1->start();
    t2->start();
    t3->start();
    t4->start();
    t5->start();
    t6->start();
    idleThread->start();
    return 0;
}
