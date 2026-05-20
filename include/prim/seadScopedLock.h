#pragma once

#include <basis/seadTypes.h>

namespace sead {

template <typename T>
class ScopedLock
{
    SEAD_NO_COPY(ScopedLock);

public:
    explicit ScopedLock(T* t)
        : mLocked(t)
    {
        mLocked->lock();
    }

    ~ScopedLock()
    {
        mLocked->unlock();
    }

private:
    T* mLocked;
};

template <typename T>
class ConditionalScopedLock
{
    SEAD_NO_COPY(ConditionalScopedLock);

public:
    ConditionalScopedLock(T* t, bool cond)
        : mLocked(nullptr)
    {
        if (cond)
        {
            mLocked = t;
            mLocked->lock();
        }
    }

    ~ConditionalScopedLock()
    {
        if (mLocked)
            mLocked->unlock();
    }

private:
    T* mLocked;
};

} // namespace sead
