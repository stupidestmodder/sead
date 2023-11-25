#include <prim/seadMemUtil.h>

#include <basis/seadRawPrint.h>
#include <heap/seadHeapMgr.h>

namespace sead {

void* MemUtil::copyAlign32(void* dst, const void* src, size_t size)
{
    SEAD_ASSERT_MSG(size % 32 == 0, "size %% 32 == 0  size: %zu", size);
    SEAD_ASSERT_MSG(PtrUtil::isAligned(dst, 32) && PtrUtil::isAligned(src, 32),
                    "pointer must be 32-byte aligned. src: 0x%p -> dst: 0x%p", src, dst);

    return copy(dst, src, size);
}

bool MemUtil::isHeap(const void* addr)
{
    if (!HeapMgr::instance())
        return false;

    return HeapMgr::isContainedInAnyHeap(addr);
}

// TODO
/*void MemUtil::dumpMemoryBinary(const void* addr, const u32 sizeFront, const u32 sizeBack, const bool isAlign)
{
    SEAD_ASSERT(static_cast<u64>(sizeFront) + static_cast<u64>(sizeBack) < UINT_MAX - 16);

    SEAD_PRINT("Memory Dump: 0x%p\n", addr);

    FixedSafeString<64> str;
    str.append(' ', 10);

    SEAD_PRINT("   %s   00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F   0123456789ABCDEF\n", str.cstr());
}*/

} // namespace sead
