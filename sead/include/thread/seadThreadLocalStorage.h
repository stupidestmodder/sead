#pragma once

#include <basis/seadTypes.h>

#if defined(SEAD_PLATFORM_WINDOWS)
#include <basis/win/seadWindows.h>
#elif defined(SEAD_PLATFORM_SDL)
#include <basis/sdl/seadSDL.h>
#endif // SEAD_PLATFORM_WINDOWS

namespace sead {

class ThreadLocalStorage
{
    SEAD_NO_COPY(ThreadLocalStorage);

public:
    ThreadLocalStorage();
    ~ThreadLocalStorage();

    void setValue(uintptr_t value);
    uintptr_t getValue() const;

private:
#if defined(SEAD_PLATFORM_WINDOWS)
    DWORD mTlsInner;
#elif defined(SEAD_PLATFORM_SDL) 
    mutable SDL_TLSID mTlsHandle;
#else
#error "Unsupported platform"
#endif // SEAD_PLATFORM_WINDOWS
};

} // namespace sead

#if defined(SEAD_PLATFORM_WINDOWS)
#include <thread/win/seadThreadLocalStorageWin.hpp>
#elif defined(SEAD_PLATFORM_SDL)
#include <thread/sdl/seadThreadLocalStorageSDL.hpp>
#else
#error "Unsupported platform"
#endif // SEAD_PLATFORM_WINDOWS
