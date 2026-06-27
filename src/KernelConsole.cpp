#include "../h/KernelConsole.hpp"
#include "../h/Syscall_c_api.hpp"

char KernelConsole::putcDataBuffer[BUFFER_SIZE];
char KernelConsole::getcDataBuffer[BUFFER_SIZE];

int KernelConsole::sendDataIndex = 0;
int KernelConsole::freeSendDataIndex = 0;

int KernelConsole::receiveDataIndex = 0;
int KernelConsole::freeReceiveDataIndex = 0;

bool KernelConsole::isConsoleInterrupted = false;

_sem* KernelConsole::getcSemaphore = nullptr;
KernelConsole::KernelConsole() {
    getcSemaphore = new _sem(0);
    *((volatile uint8*)(CONSOLE_RX_DATA + 1)) = 0x01;
}
KernelConsole* KernelConsole::getInstance() {
    static KernelConsole instance;
    return &instance;
}
void KernelConsole::putc(char c) {
    KernelConsole* console = KernelConsole::getInstance();

    while (sendDataIndex == (freeSendDataIndex + 1) % BUFFER_SIZE) {
        TCB::dispatch();
    }

    console->putcDataBuffer[freeSendDataIndex] = c;
    freeSendDataIndex = (freeSendDataIndex + 1) % BUFFER_SIZE;
}
void KernelConsole::putc_handler() {
    KernelConsole* console = KernelConsole::getInstance();

    while (true) {
        while (console->sendDataIndex == console->freeSendDataIndex) {
            thread_dispatch();
        }

        uint8 status = *((char*)CONSOLE_STATUS);
        uint8 txReady = status & CONSOLE_TX_STATUS_BIT;

        if (txReady != 0) {
            *((char*)CONSOLE_TX_DATA) =
                console->putcDataBuffer[console->sendDataIndex];

            console->sendDataIndex =
                (console->sendDataIndex + 1) % BUFFER_SIZE;
        } else {
            thread_dispatch();
        }
    }
}
char KernelConsole::getc() {
    KernelConsole* console = KernelConsole::getInstance();

    getcSemaphore->wait();

    char c = console->getcDataBuffer[receiveDataIndex];
    receiveDataIndex = (receiveDataIndex + 1) % BUFFER_SIZE;

    return c;
}
void KernelConsole::getc_handler() {

    while (true) {
        thread_dispatch();
    }
}
void KernelConsole::console_handler() {
    uint8 status = *((char*)CONSOLE_STATUS);
    uint8 rxReady = status & CONSOLE_RX_STATUS_BIT;

    if (rxReady == 0) return;

    if (receiveDataIndex == (freeReceiveDataIndex + 1) % BUFFER_SIZE) return;

    getcDataBuffer[freeReceiveDataIndex] = *((char*)CONSOLE_RX_DATA);
    freeReceiveDataIndex = (freeReceiveDataIndex + 1) % BUFFER_SIZE;
    getcSemaphore->signal();
}
void KernelConsole::getc_handler_wrapper(void*) {
    getc_handler();
}

void KernelConsole::putc_handler_wrapper(void*) {
    putc_handler();
}