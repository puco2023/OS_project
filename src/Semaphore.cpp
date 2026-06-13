#include"../h/Semaphore.hpp"
#include "../h/MemoryAllocator.hpp"
_sem::_sem(unsigned init)
    : value((int)init), closed(false), head(nullptr),tail(nullptr)
{
}
void _sem::block(unsigned n)
{
    TCB* running = TCB::running;
    running->setBlocked(true);

    BlockedSemNode* node = (BlockedSemNode*)MemoryAllocator::mem_alloc(sizeof(BlockedSemNode));
    node->thread = running;
    node->requested = n;
    node->next = nullptr;

    if (tail == nullptr) {
        head = tail = node;
    } else {
        tail->next = node;
        tail = node;
    }
}
int _sem::signal_n(unsigned n)
{
    if (closed) return -1;
    if (n == 0) return 0;

    value += n;

    while (head != nullptr && value >= (int)head->requested) {
        BlockedSemNode* node = head;
        head = head->next;

        if (head == nullptr) {
            tail = nullptr;
        }

        value -= node->requested;

        TCB* t = node->thread;
        t->setBlocked(false);
        Scheduler::put(t);

        MemoryAllocator::mem_free(node);
    }

    return 0;
}
void _sem::unblockReady()
{
    while (head != nullptr && value >= (int)head->requested) {
        BlockedSemNode* node = head;
        head = head->next;

        if (head == nullptr) {
            tail = nullptr;
        }

        value -= node->requested;

        TCB* t = node->thread;
        t->setBlocked(false);
        Scheduler::put(t);

        MemoryAllocator::mem_free(node);
    }
}
int _sem::wait_n(unsigned n)
{
    if (closed) return -1;
    if (n == 0) return 0;

    if (value >= (int)n) {
        value -= n;
        return 0;
    }

    block(n);
    TCB::dispatch();


    return 0;
}
int _sem::wait()
{
    return wait_n(1);
}

int _sem::signal()
{
    return signal_n(1);
}
int _sem::close()
{
    if (closed) return -1;

    closed = true;

    while (head != nullptr) {
        BlockedSemNode* node = head;
        head = head->next;

        TCB* t = node->thread;
        t->setBlocked(false);
        Scheduler::put(t);

        MemoryAllocator::mem_free(node);
    }

    tail = nullptr;

    return 0;
}
