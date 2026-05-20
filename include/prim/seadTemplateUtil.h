#pragma once

#include <basis/seadTypes.h>

// TODO: Add more stuff ?

namespace sead {

template <typename T, u32 S>
s32 GetArrayLength(const T (&array)[S])
{
    return S;
}

} // namespace sead
