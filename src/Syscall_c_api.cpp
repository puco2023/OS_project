#include "../h/SyscallCodes.hpp"
#include "../lib/hw.h"
#include"../h/Syscall_c_api.hpp"
#include "../lib/console.h"
void* mem_alloc(size_t size)
{
    __asm__ volatile("mv a1, %0": : "r"(size));
    __asm__ volatile("mv a0, %0": : "r"(MEM_ALLOC));
    __asm__ volatile("ecall");
    void* ret;
    __asm__ volatile("mv %0, a0": "=r"(ret));
    return ret;
}
int mem_free(void* ptr)
{
    __asm__ volatile("mv a1, %0": : "r"(ptr));
    __asm__ volatile("mv a0, %0": : "r"(MEM_FREE));
    __asm__ volatile("ecall");
    int ret;
    __asm__ volatile("mv %0, a0":"=r"(ret));
    return ret;
}
size_t mem_get_free_space() {
    __asm__ volatile("mv a0,%0": : "r"(MEM_GET_FREE_SPACE));
    __asm__ volatile("ecall");
    size_t ret;
    __asm__ volatile("mv %0, a0":"=r"(ret));
    return ret;
}
size_t mem_get_largest_free_block() {
    __asm__ volatile("mv a0,%0": : "r"(MEM_GET_LARGEST_FREE_BLOCK));
    __asm__ volatile("ecall");
    size_t ret;
    __asm__ volatile("mv %0, a0":"=r"(ret));
    return ret;
}
int thread_create(thread_t* handle, void (*start_routine)(void*), void* arg)
{
    void* stack_space = mem_alloc(DEFAULT_STACK_SIZE);
    if (stack_space == nullptr) {
        return -1;
    }

    uint64 ret;

    __asm__ volatile(
        "mv a4, %1\n\t"
        "mv a3, %2\n\t"
        "mv a2, %3\n\t"
        "mv a1, %4\n\t"
        "mv a0, %5\n\t"
        "ecall\n\t"
        "mv %0, a0"
        : "=r"(ret)
        : "r"(stack_space), "r"(arg), "r"(start_routine), "r"(handle), "r"((uint64)THREAD_CREATE)
        : "a0", "a1", "a2", "a3", "a4", "memory"
    );

    return (int)ret;
}

void thread_dispatch()
{
    __asm__ volatile("mv a0, %0" : : "r"(THREAD_DISPATCH));
    __asm__ volatile("ecall");
}
int thread_exit()
{
    uint64 code = THREAD_EXIT;

    __asm__ volatile("mv a0, %0" : : "r"(code));
    __asm__ volatile("ecall");

    int ret;
    __asm__ volatile("mv %0, a0" : "=r"(ret));

    return ret;
}
int sem_open(sem_t* handle, unsigned init)
{
    uint64 ret;

    __asm__ volatile(
        "mv a1, %1\n\t"
        "mv a2, %2\n\t"
        "mv a0, %3\n\t"
        "ecall\n\t"
        "mv %0, a0"
        : "=r"(ret)
        : "r"(handle), "r"((uint64)init), "r"((uint64)SEM_OPEN)
        : "a0", "a1", "a2", "memory"
    );

    return (int)ret;
}
int sem_wait(sem_t id)
{
    uint64 ret;

    __asm__ volatile("mv a1, %0" : : "r"(id));
    __asm__ volatile("mv a0, %0" : : "r"(SEM_WAIT));

    __asm__ volatile("ecall");

    __asm__ volatile("mv %0, a0" : "=r"(ret));

    return (int)ret;
}
int sem_close(sem_t id)
{
    uint64 ret;

    __asm__ volatile("mv a1, %0" : : "r"(id));
    __asm__ volatile("mv a0, %0" : : "r"(SEM_CLOSE));

    __asm__ volatile("ecall");

    __asm__ volatile("mv %0, a0" : "=r"(ret));

    return (int)ret;
}
int sem_signal(sem_t id)
{
    uint64 ret;

    __asm__ volatile("mv a1, %0" : : "r"(id));
    __asm__ volatile("mv a0, %0" : : "r"(SEM_SIGNAL));

    __asm__ volatile("ecall");

    __asm__ volatile("mv %0, a0" : "=r"(ret));

    return (int)ret;
}

int sem_wait_n(sem_t id, unsigned n)
{
    uint64 ret;
    __asm__ volatile(
        "mv a1, %1\n\t"
        "mv a2, %2\n\t"
        "mv a0, %3\n\t"
        "ecall\n\t"
        "mv %0, a0"
        : "=r"(ret)
        : "r"(id), "r"((uint64)n), "r"((uint64)SEM_WAIT_N)
        : "a0", "a1", "a2", "memory"
    );
    return (int)ret;
}

int sem_signal_n(sem_t id, unsigned n)
{
    uint64 ret;
    __asm__ volatile(
        "mv a1, %1\n\t"
        "mv a2, %2\n\t"
        "mv a0, %3\n\t"
        "ecall\n\t"
        "mv %0, a0"
        : "=r"(ret)
        : "r"(id), "r"((uint64)n), "r"((uint64)SEM_SIGNAL_N)
        : "a0", "a1", "a2", "memory"
    );
    return (int)ret;
}
int time_sleep(time_t time)
{
    register uint64 a0 asm("a0") = TIME_SLEEP;
    register uint64 a1 asm("a1") = (uint64)time;
    asm volatile("ecall"
        : "+r"(a0)
        : "r"(a1)
        : "memory"
    );

    return (int)a0;
}
char getc() {
    __asm__ volatile("li a0, 0x41");
    __asm__ volatile("ecall");

    char ret;
    __asm__ volatile("mv %0, a0" : "=r"(ret));

    return ret;
}

void putc(char c) {
    __asm__ volatile("mv a1, %0" : : "r"(c));
    __asm__ volatile("li a0, 0x42");
    __asm__ volatile("ecall");
}
void pair(thread_t t1,thread_t t2) {
    __asm__ volatile(
        "mv a1, %0\n\t"
        "mv a2, %1\n\t"
        "li a0, 0x14\n\t"
        "ecall"
        :
        : "r"(t1), "r"(t2)
        : "a0", "a1", "a2", "memory"
    );
}
void _sync() {
    __asm__ volatile("li a0,0x15");
    __asm__ volatile("ecall");
}