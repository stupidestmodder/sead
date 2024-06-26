#pragma once

#include <heap/seadDisposer.h>

#ifdef SEAD_PLATFORM_WINDOWS
#include <basis/win/seadWindows.h>
#endif // SEAD_PLATFORM_WINDOWS

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

protected:
#ifdef SEAD_PLATFORM_WINDOWS
    CRITICAL_SECTION mMutexInner;
#else
#error "Unsupported platform"
#endif // SEAD_PLATFORM_WINDOWS
};

} // namespace sead
