//
// Created by os on 5/11/26.
//

#include "../h/TCB.hpp"
#include "../h/RiscV.hpp"
#include "../h/Scheduler.hpp"
#include "../h/MemoryAllocator.hpp"
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
int TCB::sleep(time_t time)
{
    if (time == 0) return 0;

    TCB* old = TCB::running;

    old->sleepTime = time;
    old->nextSleeping = sleepingHead;
    sleepingHead = old;

    old->setBlocked(true);

    TCB::dispatch();
    return 0;
}
void TCB::tickSleeping()
{
    TCB* prev = nullptr;
    TCB* cur = sleepingHead;

    while (cur != nullptr) {
        if (cur->sleepTime > 0) {
            cur->sleepTime--;
        }

        if (cur->sleepTime == 0) {
            TCB* wake = cur;

            cur = cur->nextSleeping;

            if (prev == nullptr) {
                sleepingHead = cur;
            } else {
                prev->nextSleeping = cur;
            }

            wake->nextSleeping = nullptr;
            wake->setBlocked(false);
            Scheduler::put(wake);
        } else {

            prev = cur;
            cur = cur->nextSleeping;
        }
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

    idleThread = new TCB(idleBody, nullptr, stack_space, DEFAULT_TIME_SLICE);
}

TCB* TCB::createMainThread()
{
    initIdleThread();
    return new TCB(nullptr, nullptr, nullptr, DEFAULT_TIME_SLICE);
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

    running->setFinished(true);
    TCB::yield();
}
int TCB::createThread(TCB** handle, Body body, void* arg, void* stack_space)
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

    TCB* t = new TCB(body, arg, stack_space, DEFAULT_TIME_SLICE);

    if (t == nullptr) {
        return -4;
    }

    *handle = t;
    Scheduler::put(t);
    return 0;
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

    TCB* next = Scheduler::get();

    if (next == nullptr) {
        return -3;
    }

    Scheduler::put(next);

    running->setFinished(true);
    timeSliceCounter = 0;

    dispatch();

    return 1;
}
