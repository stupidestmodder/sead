#pragma once

#include <basis/seadTypes.h>

namespace sead {

class MemUtil
{
public:
    static void* fill(void* dst, u8 val, size_t size);
    static void* fillZero(void* dst, size_t size);
    static void* copyOverlap(void* dst, const void* src, size_t size);
    static void* copy(void* dst, const void* src, size_t size);
    static void* copyAlign32(void* dst, const void* src, size_t size);
    static s32 compare(const void* addr1, const void* addr2, size_t size);
    static bool isStack(const void* addr);
    static bool isHeap(const void* addr);
    static void dumpMemoryBinary(const void* addr, const u32 sizeFront, const u32 sizeBack, const bool isAlign);
};

} // namespace sead

#ifdef SEAD_PLATFORM_WINDOWS
#include <prim/win/seadMemUtilWin.hpp>
#else
#error "Unsupported platform"
#endif // SEAD_PLATFORM_WINDOWS
