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

    return 1;
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
    if (value>=0) {
        _sem* first = this->removeFirstFromList();
        if (first != nullptr) {
            if (first->hasWaiters()) {
                this->addLastInList(first);
                return first->signal();
            }
            this->addLastInList(first);
            _sem* it;
            while ((it=this->removeFirstFromList())!=first)
            {
                if (it->hasWaiters()) {
                    this->addLastInList(it);
                    return it->signal();

                }
                this->addLastInList(it);
            }
            this->addLastInList(it);
        }
    }
    return signal_n(1);
}
bool _sem::hasWaiters()
{
    return head != nullptr;
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

void _sem::addLastInList(_sem* s1) {
    this->list.addLast(s1);
}

_sem* _sem::removeFirstFromList() {
    _sem* s1 = this->list.removeFirst();
    return s1;
}

void _sem::pair(_sem* s1, _sem* s2) {
    s1->addLastInList(s2);
    s2->addLastInList(s1);
}
