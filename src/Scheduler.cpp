#include "../h/Scheduler.hpp"

#include "../h/TCB.hpp"
List<TCB> Scheduler::readyThreadQueue;
TCB* Scheduler::get() {
    TCB* out,*first,*curr;
    out=nullptr;
    curr=nullptr;
    first=nullptr;
    first = readyThreadQueue.removeFirst();
    readyThreadQueue.addFirst(first);
    bool firstTurn=true;
    while ((curr = readyThreadQueue.removeFirst())) {
        if (curr==out) {
            if (firstTurn) {
                firstTurn=false;
            }
            else {
                break;
            }
        }
        if (out==nullptr)
            out = curr;
        else if (curr->getPriority()>out->getPriority()) {
            out = curr;
        }
        readyThreadQueue.addLast(curr);
    }
    readyThreadQueue.removeElement(out);
    return out;

}
void Scheduler::put(TCB* tcb) {
    readyThreadQueue.addLast(tcb);
}