#include "../h/MemoryAllocator.hpp"
#include "../h/RiscV.hpp"
#include "../h/TCB.hpp"
#include "../h/syscall_cpp.hpp"
#include "../h/KernelConsole.hpp"
#include "../lib/hw.h"
static void idle(void* arg) {
    while (true) {
        Thread::dispatch();
    }
}
class MyPeriodicThread : public PeriodicThread {
private:
    char c;
public:
    MyPeriodicThread(time_t period,char chr) : PeriodicThread(period),c(chr) {}
protected:
    void periodicActivation() override {
        putc(c);
    }
};


int main() {
    RiscV::w_stvec((uint64) &RiscV::supervisorTrap);
    MemoryAllocator::init();

    TCB::running = TCB::createMainThread();

    KernelConsole::getInstance();

    RiscV::ms_sie(RiscV::SIE_SSIE | RiscV::SIE_SEIE);
    RiscV::ms_sstatus(RiscV::SSTATUS_SIE);

    Thread* idleThread = new Thread(idle, nullptr);

    Thread* putcHandlerThread =
        new Thread(KernelConsole::putc_handler_wrapper, nullptr);
    putcHandlerThread->start();

    idleThread->start();
    Thread* thr = new Thread(PeriodicThread::getCThread, nullptr);
    thr->start();
    MyPeriodicThread* pt1 = new MyPeriodicThread(20,'a');
    MyPeriodicThread* pt2 = new MyPeriodicThread(20,'b');
    pt1->start();
    pt2->start();
    return 0;
}
