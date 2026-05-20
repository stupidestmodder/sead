#pragma once

#include <thread/seadAtomic.h>

namespace sead { namespace hostio {

class ThreadLock
{
public:
    ThreadLock();
    ~ThreadLock();

    static void lock();
    static void unlock();

    //? Official name not known
    static u32 getLockCnt()
    {
        return sLockCnt.getValue();
    }

private:
    static AtomicU32 sLockCnt;
};

} } // namespace sead::hostio
