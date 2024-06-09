#pragma once

#include <basis/seadAssert.h>
#include <basis/seadTypes.h>

namespace sead {

template <typename T, s32 N>
class SafeArray;

template <typename T>
class SafeArray<T, 0>
{
};

// TODO
template <typename T, s32 N>
class SafeArray
{
public:
    T& operator[](s32 x)
    {
        if (static_cast<u32>(x) >= static_cast<u32>(N))
        {
            SEAD_ASSERT_MSG(false, "range over [0, %d) : %d", N, x);
            return mBuffer[0];
        }

        return mBuffer[x];
    }

    const T& operator[](s32 x) const
    {
        if (static_cast<u32>(x) >= static_cast<u32>(N))
        {
            SEAD_ASSERT_MSG(false, "range over [0, %d) : %d", N, x);
            return mBuffer[0];
        }

        return mBuffer[x];
    }

    s32 size() const { return N; }

private:
    T mBuffer[N];
};

} // namespace sead
