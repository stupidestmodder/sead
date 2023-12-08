#pragma once

#include <basis/seadAssert.h>
#include <thread/seadThreadLocalStorage.h>

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

namespace sead {

inline ThreadLocalStorage::ThreadLocalStorage()
    : mTlsInner(TlsAlloc())
{
    SEAD_ASSERT_MSG(mTlsInner != TLS_OUT_OF_INDEXES, "TlsAlloc failed");
    setValue(reinterpret_cast<uintptr_t>(nullptr));
}

inline ThreadLocalStorage::~ThreadLocalStorage()
{
    bool success = TlsFree(mTlsInner);
    SEAD_ASSERT_MSG(success, "TlsFree failed");
}

inline void ThreadLocalStorage::setValue(uintptr_t value)
{
    bool success = TlsSetValue(mTlsInner, reinterpret_cast<LPVOID>(value));
    SEAD_ASSERT_MSG(success, "TlsSetValue failed");
}

inline uintptr_t ThreadLocalStorage::getValue() const
{
    return reinterpret_cast<uintptr_t>(TlsGetValue(mTlsInner));
}

} // namespace sead
