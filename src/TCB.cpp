#include "../h/TCB.hpp"
#include "../h/RiscV.hpp"
#include "../h/Scheduler.hpp"
#include "../h/MemoryAllocator.hpp"
#include "../lib/console.h"
TCB* TCB::running = nullptr;
uint64 TCB::timeSliceCounter = 0;
TCB* TCB::idleThread = nullptr;
TCB* TCB::sleepingHead = nullptr;
<<<<<<< Updated upstream

int TCB::maxThreads=0;
int TCB::intervalTime=0;
int TCB::maxTime=0;
int TCB::currThreads=0;
List<TCB> TCB::niz;
bool TCB::isModeOn = false;
=======
List<TCB> TCB::threads;
int TCB::numberOfThreads=0;
int TCB::maxTime=0;
int TCB::intervalTime=0;
bool TCB::modeIsOn=false;
int TCB::currMade=0;
>>>>>>> Stashed changes
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
    RiscV::mc_sstatus(RiscV::SSTATUS_SIE);

    TCB* old = TCB::running;

    if (!old->isFinished() && !old->isBlocked()) {
        Scheduler::put(old);
    }

    TCB* next = Scheduler::get();

    if (next == nullptr) {
        next = idleThread;
    }

    if (next == nullptr) {
        RiscV::ms_sstatus(RiscV::SSTATUS_SIE);
        return;
    }

    if (next == old) {
        RiscV::ms_sstatus(RiscV::SSTATUS_SIE);
        return;
    }

    TCB::running = next;

    TCB::contextSwitch(&old->context, &next->context);

    RiscV::ms_sstatus(RiscV::SSTATUS_SIE);
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
    if (TCB::modeIsOn==true) {
        if (TCB::currMade-1==TCB::numberOfThreads) {
            TCB::threads.addLast(t);
            return 0;
        }
        else {
            TCB::currMade++;

        }
    }
    if (t == nullptr) {
        return -4;
    }
    bool isCounterThread = (body == &TCB::counter);

    if (TCB::isModeOn && !isCounterThread && TCB::currThreads >= TCB::maxThreads) {
        TCB::niz.addLast(t);
    } else {
        if (TCB::isModeOn && !isCounterThread) {
            TCB::currThreads++;
        }
        Scheduler::put(t);
    }

    *handle = t;
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
<<<<<<< Updated upstream
void TCB::counter(void* arg) {
    TCB::sleep(maxTime);

    TCB* t = nullptr;
    while ((t = niz.removeFirst()) != nullptr) {
        Scheduler::put(t);
        TCB::sleep(TCB::intervalTime);
    }
    TCB::maxThreads=0;
    TCB::maxTime=0;
    TCB::intervalTime=0;
    TCB::currThreads=0;
    TCB::isModeOn=false;
}
void TCB::setMaximumThreads(int num_of_threads,int max_time,int interval_time) {
    TCB::maxThreads=num_of_threads;
    TCB::maxTime=max_time;
    TCB::intervalTime=interval_time;
    TCB::currThreads=0;
    void* stack = MemoryAllocator::mem_alloc(DEFAULT_STACK_SIZE);
    TCB* t;
    createThread(&t,counter,nullptr,stack);
    TCB::isModeOn=true;
=======
void TCB::startClock(void*) {
    TCB::sleep(TCB::maxTime);
    TCB* t=nullptr;
    while ((t = threads.removeFirst())!=nullptr) {
        TCB::sleep(TCB::intervalTime);
        Scheduler::put(t);
    }
    TCB::modeIsOn=false;
}

void TCB::setMaximumThreads(int numofthreads,int maxtime,int intervaltime) {
    TCB::numberOfThreads = numofthreads;
    TCB::maxTime = maxtime;
    TCB::intervalTime = intervaltime;
    TCB::modeIsOn=true;
    TCB::currMade=0;
    void* stack = MemoryAllocator::mem_alloc(DEFAULT_STACK_SIZE);
    Scheduler::put(new TCB(&startClock,nullptr,stack,DEFAULT_TIME_SLICE));
>>>>>>> Stashed changes
}
