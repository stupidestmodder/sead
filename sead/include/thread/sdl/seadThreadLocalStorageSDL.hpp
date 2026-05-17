#pragma once

#include <basis/seadAssert.h>
#include <thread/seadThreadLocalStorage.h>

namespace sead {

static inline void TlsDtorCallback(void* value) 
{



}
inline ThreadLocalStorage::ThreadLocalStorage()
    : mTlsHandle(0)
{
}

inline ThreadLocalStorage::~ThreadLocalStorage()
{
    SDL_CleanupTLS();
   // SEAD_ASSERT_MSG(success, "TlsFree failed");
}

inline void ThreadLocalStorage::setValue(uintptr_t value)
{
    bool ret = SDL_SetTLS(&mTlsHandle, reinterpret_cast<void*>(value), nullptr);
    SEAD_ASSERT_MSG(ret == 1, "SDL_SetTLS failed");
}

inline uintptr_t ThreadLocalStorage::getValue() const
{
    return reinterpret_cast<uintptr_t>(SDL_GetTLS(&mTlsHandle));
}

} // namespace sead
