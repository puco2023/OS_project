#include "../h/TCB.hpp"
#include "../h/RiscV.hpp"
#include "../h/Scheduler.hpp"
#include "../h/MemoryAllocator.hpp"
#include "../lib/console.h"
#include "../h/Syscall_c_api.hpp"
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

void TCB::dispatch() {
    TCB* old = TCB::running;

    if (!old->isFinished() && !old->isBlocked()) {
        Scheduler::put(old);
    }
    if (old->isFinished() && old->parrent!=nullptr) {
        old->parrent->isChildFinished = true;
        old->parrent->semaphore->signal();
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
    if (running->parrent!=nullptr && running->parrent->sem!=nullptr) {
        running->parrent->sem->signal();
        delete running->parrent->sem;
        running->parrent->sem = nullptr;
    }
    running->setFinished(true);
    timeSliceCounter = 0;

    dispatch();

    return 1;
}
<<<<<<< HEAD
void TCB::join(TCB* parent, TCB* child, int time) {
    (void)time;

    if (parent == nullptr || child == nullptr || parent == child) {
        return;
    }

    while (!child->isFinished()) {
        dispatch();
    }
}
=======
namespace {
struct JoinTimerArg {
    int time;
    TCB* target;
    TCB* caller;
};
}

void TCB::timer(void* arg) {
    JoinTimerArg* joinArg = (JoinTimerArg*)arg;
    int time = joinArg->time;
    TCB* t1 = joinArg->target;
    TCB* caller = joinArg->caller;
    MemoryAllocator::mem_free(joinArg);

    ::time_sleep((time_t)time);

    if (!t1->isFinished() && caller->sem != nullptr) {
        caller->sem->signal();
        delete caller->sem;
        caller->sem = nullptr;
    }
}

void TCB::thread_join(int time,TCB* t1) {
    JoinTimerArg* joinArg = (JoinTimerArg*)MemoryAllocator::mem_alloc(sizeof(JoinTimerArg));
    joinArg->time = time;
    joinArg->target = t1;
    joinArg->caller = TCB::running;

    void* stack = MemoryAllocator::mem_alloc(DEFAULT_STACK_SIZE);
    Scheduler::put(new TCB(&timer, joinArg, stack, DEFAULT_TIME_SLICE));
    t1->parrent = TCB::running;
    if (TCB::running->sem==nullptr)
        TCB::running->sem = new _sem(0);
    TCB::running->sem->wait();
}
>>>>>>> bolja verzija
