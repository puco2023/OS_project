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

private:
    int value;
    bool closed;

    BlockedSemNode* head;
    BlockedSemNode* tail;

    List<TCB> blocked;

    void block(unsigned n);
    void unblockReady();
};

#endif
