#include "../lib/hw.h"
#include "../h/TCB.hpp"

static uint64 fibonacci(uint64 n)
{
    if (n == 0 || n == 1) { return n; }
    if (n % 4 == 0) TCB::yield();
    return fibonacci(n - 1) + fibonacci(n - 2);
}
void workerBodyA(void* arg)
{
    TCB::yield();
}

void workerBodyB(void* arg)
{
    TCB::yield();
}
void workerBodyC()
{
    uint8 i = 0;
    for (; i < 3; i++) {}
    __asm__ ("li t1, 7");
    TCB::yield();

    uint64 t1 = 0;
    __asm__ ("mv %[t1], t1" : [t1] "=r"(t1));

    uint64 result = fibonacci(20);
    (void)t1;
    (void)result;

    for (; i < 6; i++) {}

    TCB::running->setFinished(true);
    TCB::yield();
}

void workerBodyD()
{

    uint8 i = 10;
    for (; i < 13; i++) {}
    __asm__ ("li t1, 5");
    TCB::yield();

    uint64 result = fibonacci(23);
    (void)result;

    for (; i < 16; i++) {}

    TCB::running->setFinished(true);
    TCB::yield();
}
