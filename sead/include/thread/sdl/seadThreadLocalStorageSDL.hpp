#pragma once

#include <basis/seadAssert.h>
#include <thread/seadThreadLocalStorage.h>

namespace sead {

static inline void TlsDtorCallback(void* value) 
{



}
inline ThreadLocalStorage::ThreadLocalStorage()
    : mTlsHandle(SDL_TLSCreate())
{
    SEAD_ASSERT_MSG(mTlsHandle != 0, "SDL_TLSCreate failed");
    setValue(reinterpret_cast<uintptr_t>(nullptr));
}

inline ThreadLocalStorage::~ThreadLocalStorage()
{
    SDL_TLSCleanup();
   // SEAD_ASSERT_MSG(success, "TlsFree failed");
}

inline void ThreadLocalStorage::setValue(uintptr_t value)
{
    s32 ret = SDL_TLSSet(mTlsHandle, reinterpret_cast<void*>(value), nullptr);
    SEAD_ASSERT_MSG(ret == 0, "SDL_TLSSet failed");
}

inline uintptr_t ThreadLocalStorage::getValue() const
{
    return reinterpret_cast<uintptr_t>(SDL_TLSGet(mTlsHandle));
}

} // namespace sead
