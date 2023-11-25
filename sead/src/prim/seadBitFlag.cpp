#include <prim/seadBitFlag.h>

namespace sead {

s32 BitFlagUtil::countOnBit(u32 x)
{
    x = x - ((x >> 1) & 0x55555555);
    x = (x & 0x33333333) + ((x >> 2) & 0x33333333);
    x = (x + (x >> 4)) & 0x0F0F0F0F;
    x += (x >> 8);
    x += (x >> 16);
    return x & 0x3f;
}

s32 BitFlagUtil::countContinuousOffBitFromRight(u32 x)
{
    return countOnBit((x & -static_cast<s32>(x)) - 1);
}

s32 BitFlagUtil::countRightOnBit(u32 x, s32 bit)
{
    SEAD_ASSERT(static_cast<u32>(bit) < sizeof(u32) * 8);
    const u32 mask = ((1u << bit) - 1) | (1u << bit);
    return countOnBit(x & mask);
}

s32 BitFlagUtil::findOnBitFromRight(u32 x, s32 num)
{
    SEAD_ASSERT(num > 0);

    if (x == 0)
        return -1;

    while (--num > 0)
    {
        x &= x - 1;
        if (x == 0)
            return -1;
    }

    return countContinuousOffBitFromRight(x);
}

s32 BitFlagUtil::countOnBit64(u64 x)
{
    return countOnBit(static_cast<u32>(x)) + countOnBit(static_cast<u32>(x >> 32));
}

s32 BitFlagUtil::countContinuousOffBitFromRight64(u64 x)
{
    return countOnBit64((x & -static_cast<s64>(x)) - 1);
}

s32 BitFlagUtil::countRightOnBit64(u64 x, s32 bit)
{
    SEAD_ASSERT(static_cast<u64>(bit) < sizeof(u64) * 8);
    const u64 mask = ((1ull << bit) - 1) | (1ull << bit);
    return countOnBit64(x & mask);
}

s32 BitFlagUtil::findOnBitFromRight64(u64 x, s32 num)
{
    SEAD_ASSERT(num > 0);

    if (x == 0)
        return -1;

    while (--num > 0)
    {
        x &= x - 1;
        if (x == 0)
            return -1;
    }

    return countContinuousOffBitFromRight64(x);
}

} // namespace sead
