#include "../h/syscall_cpp.hpp"
#include "../h/KernelConsole.hpp"
#include "../h/MemoryAllocator.hpp"
List<PeriodicThread>* PeriodicThread::niz = nullptr;
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
    : Thread(&PeriodicThread::periodicThreadWrapper, this),
      period(period),
      terminated(false) {
    if (niz == nullptr) {
        niz = new List<PeriodicThread>();
    }
    niz->addLast(this);
}

void PeriodicThread::terminate() {
    terminated = true;
}

void PeriodicThread::periodicThreadWrapper(void* arg) {
    PeriodicThread* thread = (PeriodicThread*) arg;

    while (!thread->terminated) {
        thread->periodicActivation();

        if (thread->terminated) {
            break;
        }

        Thread::sleep(thread->period);
    }
}
void PeriodicThread::getCThread(void* arg) {
    Thread* getcHandlerThread =
    new Thread(KernelConsole::getc_handler_wrapper, nullptr);
    getcHandlerThread->start();
    while (true) {
        char c = ::getc();
        if (c=='k') {
            if (niz != nullptr && niz->peekFirst()!=nullptr) {
                PeriodicThread::niz->peekFirst()->terminate();
                niz->removeFirst();

            }
        }
    }
}
