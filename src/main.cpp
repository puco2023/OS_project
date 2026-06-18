#include "../h/MemoryAllocator.hpp"
#include "../h/TCB.hpp"
#include "../h/RiscV.hpp"
#include "../h/KernelConsole.hpp"
#include "../h/Syscall_c_api.hpp"
#include "../h/syscall_cpp.hpp"
#include "../h/Print.hpp"
#include "../lib/hw.h"

volatile bool putcDone = false;
volatile bool echoOneDone = false;
volatile bool echoManyDone = false;
volatile bool writerDone[3] = {false, false, false};

void consolePutcTest(void*) {
    printString("\n[TEST 1] PUTC test start\n");

    printString("Ako ovo vidis, Console::putc radi.\n");
    printString("Ispisujem ABCDEFG:\n");

    Console::putc('A');
    Console::putc('B');
    Console::putc('C');
    Console::putc('D');
    Console::putc('E');
    Console::putc('F');
    Console::putc('G');
    Console::putc('\n');

    printString("[TEST 1] PUTC test gotov\n");

    putcDone = true;
    thread_exit();
}

void consoleEchoOneTest(void*) {
    printString("\n[TEST 2] GETC jedan karakter\n");
    printString("Unesi jedan karakter: ");

    char c = Console::getc();

    printString("\nUneo si: ");
    Console::putc(c);
    Console::putc('\n');

    printString("[TEST 2] GETC jedan karakter gotov\n");

    echoOneDone = true;
    thread_exit();
}

void consoleEchoManyTest(void*) {
    printString("\n[TEST 3] GETC vise karaktera\n");
    printString("Unesi 5 karaktera:\n");

    for (int i = 0; i < 5; i++) {
        char c = Console::getc();

        printString("Primljen karakter ");
        printInteger(i);
        printString(": ");
        Console::putc(c);
        Console::putc('\n');
    }

    printString("[TEST 3] GETC vise karaktera gotov\n");

    echoManyDone = true;
    thread_exit();
}

void writerThread(void* arg) {
    uint64 id = (uint64)arg;

    for (int i = 0; i < 10; i++) {
        printString("[WRITER ");
        printInteger(id);
        printString("] i=");
        printInteger(i);
        Console::putc('\n');

        thread_dispatch();
    }

    writerDone[id] = true;
    thread_exit();
}

int main() {
    MemoryAllocator::init();

    RiscV::w_stvec((uint64)&RiscV::supervisorTrap);

    TCB::running = TCB::createMainThread();

    KernelConsole::getInstance();

    RiscV::ms_sie(RiscV::SIE_SSIE | RiscV::SIE_SEIE);
    RiscV::ms_sstatus(RiscV::SSTATUS_SIE);

    thread_t putcThread;
    thread_create(&putcThread, KernelConsole::putc_handler_wrapper, nullptr);

    printString("\n===== CONSOLE TESTOVI START =====\n");

    thread_t t1;
    thread_create(&t1, consolePutcTest, nullptr);

    while (!putcDone) {
        TCB::dispatch();
    }

    printString("\n===== TEST 1 GOTOV =====\n");


    thread_t t2;
    thread_create(&t2, consoleEchoOneTest, nullptr);

    while (!echoOneDone) {
        TCB::dispatch();
    }

    printString("\n===== TEST 2 GOTOV =====\n");


    thread_t t3;
    thread_create(&t3, consoleEchoManyTest, nullptr);

    while (!echoManyDone) {
        TCB::dispatch();
    }

    printString("\n===== TEST 3 GOTOV =====\n");


    printString("\n[TEST 4] vise niti pise na konzolu\n");

    thread_t w0, w1, w2;

    thread_create(&w0, writerThread, (void*)0);
    thread_create(&w1, writerThread, (void*)1);
    thread_create(&w2, writerThread, (void*)2);

    while (!writerDone[0] || !writerDone[1] || !writerDone[2]) {
        TCB::dispatch();
    }

    printString("\n===== TEST 4 GOTOV =====\n");

    printString("\n===== SVI CONSOLE TESTOVI GOTOVI =====\n");

    for (int i = 0; i < 1000; i++) {
        TCB::dispatch();
    }

    return 0;
}