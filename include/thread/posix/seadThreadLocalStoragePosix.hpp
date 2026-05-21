#pragma once

#include <basis/seadAssert.h>
#include <thread/seadThreadLocalStorage.h>

namespace sead {

inline ThreadLocalStorage::ThreadLocalStorage()
    : mPthreadKey(0)
{
    s32 ret = pthread_key_create(&mPthreadKey, nullptr);
    SEAD_ASSERT_MSG(ret == 0, "pthread_key_create failed");
}

inline ThreadLocalStorage::~ThreadLocalStorage()
{
    s32 ret = pthread_key_delete(mPthreadKey);
    SEAD_ASSERT_MSG(ret == 0, "pthread_key_delete failed");
}

inline void ThreadLocalStorage::setValue(uintptr_t value)
{
    s32 ret = pthread_setspecific(mPthreadKey, reinterpret_cast<void*>(value));
    SEAD_ASSERT_MSG(ret == 0, "pthread_setspecific failed");
}

inline uintptr_t ThreadLocalStorage::getValue() const
{
    return reinterpret_cast<uintptr_t>(pthread_getspecific(mPthreadKey));
}

} // namespace sead
