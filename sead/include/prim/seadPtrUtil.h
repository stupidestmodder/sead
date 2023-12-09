#pragma once

#include <basis/seadAssert.h>
#include <basis/seadTypes.h>

namespace sead {

class PtrUtil
{
public:
    static void* roundUpPow2(const void* ptr, u32 alignment)
    {
        return reinterpret_cast<void*>(uintptr_t(ptr) + uintptr_t(alignment - 1) & ~uintptr_t(alignment - 1));
    }

    // TODO
    static void* roundUpN(const void*, u32);

    static void* roundDownPow2(const void* ptr, u32 alignment)
    {
        return reinterpret_cast<void*>(uintptr_t(ptr) & ~uintptr_t(alignment - 1));
    }

    // TODO
    static void* roundDownN(const void*, u32);

    static void* addOffset(const void* ptr, intptr_t offset)
    {
        return reinterpret_cast<void*>(uintptr_t(ptr) + offset);
    }

    static intptr_t diff(const void* a, const void* b)
    {
        return intptr_t(a) - intptr_t(b);
    }

    static bool isInclude(const void* ptr, const void* begin, const void* end)
    {
        return uintptr_t(begin) <= uintptr_t(ptr) && uintptr_t(ptr) < uintptr_t(end);
    }

    static bool isAligned(const void* ptr, s32 alignment)
    {
        SEAD_ASSERT(alignment != 0);
        return uintptr_t(ptr) % alignment == 0;
    }

    static bool isAlignedPow2(const void* ptr, u32 alignment)
    {
        return (uintptr_t(ptr) & uintptr_t(alignment - 1)) == 0;
    }

    // TODO
    static bool isAlignedN(const void*, s32);
};

} // namespace sead
