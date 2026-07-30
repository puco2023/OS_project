#include "../h/MemoryAllocator.hpp"
#include "../h/RiscV.hpp"
#include "../h/TCB.hpp"
#include "../h/syscall_cpp.hpp"
#include "../h/KernelConsole.hpp"
#include "../lib/hw.h"

static void printStr(const char* s) {
    while (*s) {
        Console::putc(*s++);
    }
}

class ThreadA : public Thread {
    Thread* b;
    Thread* c;
public:
    ThreadA() : Thread(), b(nullptr), c(nullptr) {}

    void setPeers(Thread* b_, Thread* c_) {
        b = b_;
        c = c_;
    }

    void run() override {
        printStr("A -> B\n");
        b->send((char*)"A -> B");

        printStr("A -> C\n");
        c->send((char*)"A -> C");

        char* odgovor = this->receive();
        printStr("primljena poruka (A): ");
        printStr(odgovor);
        printStr("\n");
    }
};

class ThreadB : public Thread {
    Thread* c;
public:
    ThreadB() : Thread(), c(nullptr) {}

    void setPeers(Thread* c_) {
        c = c_;
    }

    void run() override {
        char* odA = this->receive();
        printStr("primljena poruka (B): ");
        printStr(odA);
        printStr("\n");

        printStr("B -> C\n");
        c->send((char*)"B -> C (1)");

        printStr("B -> C\n");
        c->send((char*)"B -> C (2)");

        char* odC = this->receive();
        printStr("primljena poruka (B): ");
        printStr(odC);
        printStr("\n");
    }
};

class ThreadC : public Thread {
    Thread* a;
    Thread* b;
public:
    ThreadC() : Thread(), a(nullptr), b(nullptr) {}

    void setPeers(Thread* a_, Thread* b_) {
        a = a_;
        b = b_;
    }

    void run() override {
        for (int i = 0; i < 3; i++) {
            char* poruka = this->receive();
            printStr("primljena poruka (C): ");
            printStr(poruka);
            printStr("\n");
        }

        printStr("C -> B\n");
        b->send((char*)"C -> B");

        printStr("C -> A\n");
        a->send((char*)"C -> A");
    }
};

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

    ThreadA* a = new ThreadA();
    ThreadB* b = new ThreadB();
    ThreadC* c = new ThreadC();

    a->setPeers(b, c);
    b->setPeers(c);
    c->setPeers(a, b);

    a->start();
    b->start();
    c->start();

    idleThread->start();


    return 0;
}