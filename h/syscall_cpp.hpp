#ifndef syscall_cpp
#define syscall_cpp
#include"../lib/hw.h"
#include "../h/Syscall_c_api.hpp"
#include "../h/Semaphore.hpp"
void* operator new(size_t);
void operator delete(void*);

class Thread {
public:
    Thread(void (*body)(void*), void* arg);
    virtual ~Thread();

    int start();

    static void dispatch();
    static int sleep(time_t);

protected:
    Thread();
    virtual void run() {}

private:
    thread_t myHandle;
    void (*body)(void*);
    void* arg;

    static void wrapper(void* thread);
};
class Semaphore {
public:
    Semaphore (unsigned init = 1);
    virtual ~Semaphore ();
    int wait ();
    int signal ();
private:
    sem_t myHandle;
};
class Console {
public:
    static char getc();
    static void putc(char c);
};
#endif
