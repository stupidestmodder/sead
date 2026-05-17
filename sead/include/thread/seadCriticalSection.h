#pragma once

#include <heap/seadDisposer.h>

#if defined(SEAD_PLATFORM_WINDOWS)
#include <basis/win/seadWindows.h>
#endif // SEAD_PLATFORM_WINDOWS

#include <basis/sdl/seadSDL.h>

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
    SDL_Mutex* mMutexInner;
#if defined(SEAD_PLATFORM_WINDOWS)
    CRITICAL_SECTION mMutexInner;
#else
//s#error "Unsupported platform"
#endif // SEAD_PLATFORM_WINDOWS
};

} // namespace sead
