#include "../h/syscall_cpp.hpp"
#include "../h/MemoryAllocator.hpp"

void* operator new(size_t size)
{
    return MemoryAllocator::mem_alloc(size);
}

void operator delete(void* ptr)
{
    MemoryAllocator::mem_free(ptr);
}

Thread::Thread(void (*body)(void*), void* arg)
    : myHandle(nullptr), body(body), arg(arg)
{
}

Thread::Thread()
    : myHandle(nullptr), body(wrapper), arg(this)
{
}

Thread::~Thread()
{
}

void Thread::wrapper(void* thread)
{
    Thread* t = (Thread*) thread;
    t->run();
}
int Thread::start()
{
    return thread_create(&myHandle, body, arg);
}

void Thread::dispatch()
{
    thread_dispatch();
}

int Thread::sleep(time_t time)
{
    return time_sleep(time);
}
Semaphore::Semaphore(unsigned init)
    : myHandle(nullptr)
{
    sem_open(&myHandle, init);
}

Semaphore::~Semaphore()
{
    if (myHandle != nullptr) {
        sem_close(myHandle);
    }
}

int Semaphore::wait()
{
    return sem_wait(myHandle);
}

int Semaphore::signal()
{
    return sem_signal(myHandle);
}
char Console::getc() {
    return ::getc();
}

void Console::putc(char c) {
    ::putc(c);
}

PeriodicThread::PeriodicThread(time_t period)
    : Thread(&PeriodicThread::periodicThreadWrapper, this), period(period) {
}

void PeriodicThread::terminate() {
    period = 0;
}

void PeriodicThread::periodicThreadWrapper(void* arg) {
    PeriodicThread* thread = (PeriodicThread*) arg;

    while (thread->period != 0) {
        thread->periodicActivation();

        if (thread->period == 0) {
            break;
        }

        Thread::sleep(thread->period);
    }
}
void Thread::join(int time) {
    ::thread_join(&this->myHandle, time);
}