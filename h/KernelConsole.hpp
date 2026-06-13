#pragma once

#include "../h/Semaphore.hpp"
#include "../h/TCB.hpp"
#include "../lib/hw.h"

class KernelConsole {
private:
    static const int BUFFER_SIZE = 1024;

    static char putcDataBuffer[BUFFER_SIZE];
    static char getcDataBuffer[BUFFER_SIZE];

    static int sendDataIndex;
    static int freeSendDataIndex;

    static int receiveDataIndex;
    static int freeReceiveDataIndex;

    static bool isConsoleInterrupted;

    static _sem* getcSemaphore;

    KernelConsole();

    static void getc_handler();
    static void putc_handler();

public:
    void putc(char c);
    char getc();

    void console_handler();

    static void getc_handler_wrapper(void*);
    static void putc_handler_wrapper(void*);

    static KernelConsole* getInstance();

    KernelConsole(const KernelConsole&) = delete;
    void operator=(const KernelConsole&) = delete;
};