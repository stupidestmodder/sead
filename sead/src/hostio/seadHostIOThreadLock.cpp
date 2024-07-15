#include <hostio/seadHostIOThreadLock.h>

#include <thread/seadCriticalSection.h>

namespace sead { namespace hostio {

static CriticalSection* GetHostIOCriticalSection()
{
    static CriticalSection sHostIOCriticalSection;
    return &sHostIOCriticalSection;
}

AtomicU32 ThreadLock::sLockCnt(0);

ThreadLock::ThreadLock()
{
    ThreadLock::lock();
}

ThreadLock::~ThreadLock()
{
    ThreadLock::unlock();
}

void ThreadLock::lock()
{
    sLockCnt.increment();
    GetHostIOCriticalSection()->lock();
}

void ThreadLock::unlock()
{
    GetHostIOCriticalSection()->unlock();
    sLockCnt.decrement();
}

} } // namespace sead::hostio
