#ifndef PROJEKAT_TCB_H
#define PROJEKAT_TCB_H

#include "Scheduler.hpp"
#include "../lib/hw.h"
class TCB {
public:
    ~TCB() { delete[] stack; }

    bool isFinished() const {
        return finished;
    }

    void setFinished(bool f) {
        finished = f;
    }

    uint64 getTimeSlice() const {
        return timeSlice;
    }
    static int sleep(time_t time);
    static void tickSleeping();
    using Body = void (*)(void*);
    bool isFinished(){return finished;}
    static int createThread(TCB** handle, Body body, void* arg, void* stack_space);
    static void yield();
    void setBlocked(bool b){this->blocked=b;}
    bool isBlocked() {return blocked;}
    static TCB* running;
    static void dispatch();
    static TCB* createMainThread();
    static void initIdleThread();
    explicit TCB(Body body, void* arg, void* stack_space, uint64 timeSlice)
        : body(body),
          arg(arg),
          stack((uint64*)stack_space),
          context({
              body != nullptr ? (uint64)&threadWrapper : 0,
              stack != nullptr ? (uint64)((uint64*)stack + DEFAULT_STACK_SIZE / sizeof(uint64)) : 0,
              0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
          }),
          finished(false),
          blocked(false),
          sleepTime(0),
          nextSleeping(nullptr),
          timeSlice(timeSlice)
    {
    }
    int thread_exit();
    struct Context {
        uint64 ra;
        uint64 sp;
        uint64 s0;
        uint64 s1;
        uint64 s2;
        uint64 s3;
        uint64 s4;
        uint64 s5;
        uint64 s6;
        uint64 s7;
        uint64 s8;
        uint64 s9;
        uint64 s10;
        uint64 s11;
    };
    static void setMaximumThreads(int num_of_threads, int max_time, int interval_time);
    static void counter(void* arg);
private:


    Body body;
    void* arg;

    uint64* stack;
    Context context;

    bool finished;
    bool blocked;

    time_t sleepTime;
    TCB* nextSleeping;

    uint64 timeSlice;

    static TCB* sleepingHead;

    friend class RiscV;

    static void contextSwitch(Context* oldContext, Context* newContext);

    static uint64 constexpr STACK_SIZE = 1024;
    static uint64 constexpr TIME_SLICE = 2;

    static uint64 timeSliceCounter;
    static TCB* idleThread;

    static void threadWrapper();
    static void idleBody(void*);
    static int maxThreads;
    static int maxTime;
    static int intervalTime;
    static int currThreads;
    static List<TCB> niz;
    static bool isModeOn;
};

#endif
