#ifndef SEMAPHORE_HPP
#define SEMAPHORE_HPP

#include "../h/TCB.hpp"
struct BlockedSemNode {
    TCB* thread;
    unsigned requested;
    BlockedSemNode* next;
};
class _sem {
public:
    _sem(unsigned init = 1);
    int wait_n(unsigned n);
    int signal_n(unsigned n);
    int wait();
    int signal();
    int close();
    void pair(_sem* s1,_sem* s2);
    void addLastInList(_sem* s1);
    _sem* removeFirstFromList();

private:
    int value;
    bool closed;

    BlockedSemNode* head;
    BlockedSemNode* tail;

    List<TCB> blocked;

    void block(unsigned n);
    void unblockReady();
    bool hasWaiters();
    List<_sem> list;
};

#endif
