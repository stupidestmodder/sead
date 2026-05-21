#pragma once

#include <heap/seadDisposer.h>
#include <time/seadTickSpan.h>

#if defined(SEAD_PLATFORM_WINDOWS)
#include <basis/win/seadWindows.h>
#endif // SEAD_PLATFORM_WINDOWS

#if defined(SEAD_PLATFORM_POSIX)
#include <pthread.h>
#endif // SEAD_PLATFORM_POSIX

namespace sead {

class Event : public IDisposer
{
    SEAD_NO_COPY(Event);

public:
    Event();
    explicit Event(Heap* disposerHeap);
    explicit Event(bool manualReset);
    Event(Heap* disposerHeap, bool manualReset);

    ~Event() override;

    void initialize(bool manualReset);
    void wait();
    bool wait(TickSpan timeout);
    void setSignal();
    void resetSignal();

protected:
#if defined(SEAD_PLATFORM_WINDOWS)
    HANDLE mEventInner;
#elif defined(SEAD_PLATFORM_POSIX)
    pthread_cond_t mCond;
    pthread_mutex_t mMutexInner;
    volatile bool mIsManualReset;
    volatile bool mIsSignal;
#else
#error "Unsupported platform"
#endif // SEAD_PLATFORM_WINDOWS

#if defined(SEAD_TARGET_DEBUG)
    bool mIsInitialized;
#endif // SEAD_TARGET_DEBUG
};

} // namespace sead
