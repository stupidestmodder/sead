#pragma once

#include <heap/seadDisposer.h>

#if defined(SEAD_PLATFORM_WINDOWS)
#include <basis/win/seadWindows.h>
#endif // SEAD_PLATFORM_WINDOWS

#if defined(SEAD_PLATFORM_SDL)
#include <basis/sdl/seadSDL.h>
#endif // SEAD_PLATFORM_SDL

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
#if defined(SEAD_PLATFORM_WINDOWS)
CRITICAL_SECTION mMutexInner;
#elif defined(SEAD_PLATFORM_SDL)
    SDL_Mutex* mMutexInner;
#else
#error "Unsupported platform"
#endif // SEAD_PLATFORM_WINDOWS
};

} // namespace sead
