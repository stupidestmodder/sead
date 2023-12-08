#pragma once

#include <basis/seadTypes.h>

#ifdef SEAD_PLATFORM_WINDOWS
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
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
#ifdef SEAD_PLATFORM_WINDOWS
    DWORD mTlsInner;
#else
#error "Unsupported platform"
#endif // SEAD_PLATFORM_WINDOWS
};

} // namespace sead

#ifdef SEAD_PLATFORM_WINDOWS
#include <thread/win/seadThreadLocalStorageWin.hpp>
#else
#error "Unsupported platform"
#endif // SEAD_PLATFORM_WINDOWS
