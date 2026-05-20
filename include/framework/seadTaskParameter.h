#pragma once

#include <basis/seadAssert.h>
#include <prim/seadRuntimeTypeInfo.h>

namespace sead {

class TaskParameter
{
    SEAD_RTTI_BASE(TaskParameter);

public:
    template <typename T>
    T* cast()
    {
        T* ptr = DynamicCast<T>(this);
        SEAD_ASSERT(ptr);
        return ptr;
    }
};

} // namespace sead
