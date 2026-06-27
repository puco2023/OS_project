#ifndef PROJEKAT_SCHEDULER_H
#define PROJEKAT_SCHEDULER_H

class TCB;
#include "list.hpp"
class Scheduler
{
private:
        static List<TCB> readyThreadQueue;
public:
    static TCB *get();

    static void put(TCB *tcb);
};
#endif //PROJEKAT_SCHEDULER_H
