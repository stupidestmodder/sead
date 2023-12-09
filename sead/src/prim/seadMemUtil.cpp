#include <prim/seadMemUtil.h>

#include <basis/seadRawPrint.h>
#include <heap/seadHeapMgr.h>

static char toPrintable(char c)
{
    if (c < ' ' || c > '~') 
        c = '.';

    return c;
}

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

void MemUtil::dumpMemoryBinary(const void* addr, const u32 sizeFront, const u32 sizeBack, const bool isAlign)
{
    const u32 cBytesPerRow = 16;
    const u32 cIndentSpaces = 2;
    const u32 cAddressPrintWidth = sizeof(void*) * 2;

    SEAD_ASSERT(static_cast<u64>(sizeFront) + static_cast<u64>(sizeBack) < UINT_MAX - cBytesPerRow);

    SEAD_PRINT("Memory Dump: 0x%p\n", addr);

    FixedSafeString<64> indent;
    indent.append(' ', cIndentSpaces + cAddressPrintWidth);

    SEAD_PRINT("   %s   00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F   0123456789ABCDEF\n", indent.cstr());

    const void* alignedAddr = isAlign ? PtrUtil::roundDownPow2(addr, cBytesPerRow) : addr;
    intptr_t addrDiff = PtrUtil::diff(addr, alignedAddr);

    u32 offset = addrDiff < sizeFront ? (sizeFront - addrDiff - 1) / cBytesPerRow + 1 : 0;
    u32 numChunks = sizeBack + addrDiff < cBytesPerRow ? 0 : (sizeBack + addrDiff) / cBytesPerRow;

    u8* data = static_cast<u8*>(PtrUtil::addOffset(alignedAddr, offset * -cBytesPerRow));
    for (u32 i = 0; i < numChunks + offset + 1; i++)
    {
        const char* lineMarker = i == offset ? "*" : " ";

        SEAD_PRINT(" %s 0x%p   %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X   %c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c\n",
                    lineMarker, data,
                    data[0], data[1], data[2],  data[3],  data[4],  data[5],  data[6],  data[7],
                    data[8], data[9], data[10], data[11], data[12], data[13], data[14], data[15],
                    toPrintable(data[0]),  toPrintable(data[1]),  toPrintable(data[2]),  toPrintable(data[3]),
                    toPrintable(data[4]),  toPrintable(data[5]),  toPrintable(data[6]),  toPrintable(data[7]),
                    toPrintable(data[8]),  toPrintable(data[9]),  toPrintable(data[10]), toPrintable(data[11]),
                    toPrintable(data[12]), toPrintable(data[13]), toPrintable(data[14]), toPrintable(data[15]));

        data += 16;
    }
}

} // namespace sead
