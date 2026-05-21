#pragma once

#include <heap/seadDisposer.h>

#if defined(SEAD_PLATFORM_WINDOWS)
#include <basis/win/seadWindows.h>
#endif // SEAD_PLATFORM_WINDOWS

#if defined(SEAD_PLATFORM_POSIX)
#include <pthread.h>
#endif // SEAD_PLATFORM_POSIX

namespace sead {

class StackSymbolResolver;

class CriticalSection : public IDisposer
{
    SEAD_NO_COPY(CriticalSection);

public:
    CriticalSection();
    explicit CriticalSection(Heap* disposerHeap);
    ~CriticalSection() override;

    void lock();
    bool tryLock();
    void unlock();

    void dump(StackSymbolResolver*); // TODO

#if defined(SEAD_PLATFORM_POSIX)
protected:
    void initialize_pthread(pthread_mutex_t* pthread);
#endif // SEAD_PLATFORM_POSIX

protected:
#if defined(SEAD_PLATFORM_WINDOWS)
    CRITICAL_SECTION mMutexInner;
#elif defined(SEAD_PLATFORM_POSIX)
    pthread_mutex_t mMutexInner;
#else
#error "Unsupported platform"
#endif // SEAD_PLATFORM_WINDOWS
};

} // namespace sead
