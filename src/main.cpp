#include "../h/MemoryAllocator.hpp"
#include "../h/TCB.hpp"
#include "../h/RiscV.hpp"
#include "../h/KernelConsole.hpp"
#include "../h/Syscall_c_api.hpp"
#include "../h/syscall_cpp.hpp"
#include "../lib/hw.h"
volatile bool done = false;

void consoleTest(void*) {
    char o = Console::getc();
    Console::putc(o);
    Console::putc('\n');

    done = true;
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

    thread_t testThread;
    thread_create(&testThread, consoleTest, nullptr);

    while (!done) {
        TCB::dispatch();
    }

    for (int i = 0; i < 1000; i++) {
        TCB::dispatch();
    }

    return 0;
}