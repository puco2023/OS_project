#include "../h/TCB.hpp"
#include "../h/RiscV.hpp"
#include "../h/Scheduler.hpp"
#include "../h/MemoryAllocator.hpp"
#include "../lib/console.h"
TCB* TCB::running = nullptr;
uint64 TCB::timeSliceCounter = 0;
TCB* TCB::idleThread = nullptr;
TCB* TCB::sleepingHead = nullptr;
void TCB::idleBody(void*)
{
    while (true) {
        TCB::dispatch();
    }
}
void TCB::initIdleThread()
{
    if (idleThread != nullptr) {
        return;
    }

    void* stack_space = MemoryAllocator::mem_alloc(DEFAULT_STACK_SIZE);
    if (stack_space == nullptr) {
        return;
    }

    idleThread = new TCB(idleBody, nullptr, stack_space, DEFAULT_TIME_SLICE,low);
}

TCB* TCB::createMainThread()
{
    initIdleThread();
    return new TCB(nullptr, nullptr, nullptr, DEFAULT_TIME_SLICE,low);
}

void TCB::yield()
{
    __asm__ volatile("li a0, 0x13");
    __asm__ volatile("ecall");
}

void TCB::dispatch()
{
    TCB* old = TCB::running;

    if (!old->isFinished() && !old->isBlocked()) {
        Scheduler::put(old);
    }

    TCB* next = Scheduler::get();

    if (next == nullptr) {
        next = idleThread;
    }

    if (next == nullptr) {
        return;
    }

    if (next == old) {
        return;
    }

    TCB::running = next;

    TCB::contextSwitch(&old->context, &next->context);
}

void TCB::threadWrapper()
{
    RiscV::popSppSpie();
    running->body(running->arg);
    __asm__ volatile("li a0, 0x12");
    __asm__ volatile("ecall");
}
int TCB::createThread(TCB** handle, Body body, void* arg, void* stack_space,Priority p)
{
    if (handle == nullptr) {
        return -1;
    }

    if (body == nullptr) {
        return -2;
    }

    if (stack_space == nullptr) {
        return -3;
    }

    TCB* t = new TCB(body, arg, stack_space, DEFAULT_TIME_SLICE,p);

    if (t == nullptr) {
        return -4;
    }

    *handle = t;
    Scheduler::put(t);
    return 0;
}


int TCB::sleep(time_t time) {
    if (time == 0) return 0;
    running->sleepTime = time;
    running->nextSleeping = sleepingHead;
    sleepingHead = running;

    running->setBlocked(true);
    dispatch();

    return 0;
}

void TCB::tickSleeping() {
    TCB* prev = nullptr;
    TCB* curr = sleepingHead;
    while (curr != nullptr) {
        curr->sleepTime--;
        if (curr->sleepTime == 0) {
            TCB* next = curr->nextSleeping;
            if (prev == nullptr) sleepingHead = next;
            else prev->nextSleeping = next;
            curr->nextSleeping = nullptr;
            curr->setBlocked(false);
            Scheduler::put(curr);
            curr = next;
        } else {
            prev = curr;
            curr = curr->nextSleeping;
        }
    }
}

int TCB::thread_exit()
{
    if (running == nullptr) {
        return -1;
    }

    if (running->isFinished()) {
        return -2;
    }
    if (running->isBlocked())
    {return -3;}

    running->setFinished(true);
    timeSliceCounter = 0;

    dispatch();

    return 1;
}
