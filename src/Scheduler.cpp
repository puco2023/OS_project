//
// Created by os on 5/11/26.
//

#include "../h/Scheduler.hpp"
List<TCB> Scheduler::readyCoroutineQueue;
TCB* Scheduler::get() {
    return readyCoroutineQueue.removeFirst();
}
void Scheduler::put(TCB* tcb) {
    readyCoroutineQueue.addLast(tcb);
}