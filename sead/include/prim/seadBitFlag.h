#pragma once

#include <basis/seadAssert.h>
#include <basis/seadTypes.h>

namespace sead {

class BitFlagUtil
{
public:
    static s32 countOnBit(u32 x);
    static s32 countContinuousOffBitFromRight(u32 x);
    static s32 countRightOnBit(u32 x, s32 bit);
    static s32 findOnBitFromRight(u32 x, s32 num);
    static s32 countOnBit64(u64 x);
    static s32 countContinuousOffBitFromRight64(u64 x);
    static s32 countRightOnBit64(u64 x, s32 bit);
    static s32 findOnBitFromRight64(u64 x, s32 num);
};

template <typename T>
class BitFlag
{
public:
    static const u32 cBitNum = sizeof(T) * 8;

public:
    BitFlag()
    {
        makeAllZero();
    }

    explicit BitFlag(T t)
        : mBits(t)
    {
    }

    operator T() const { return mBits; }

    void makeAllZero() { mBits = static_cast<T>(0); }
    void makeAllOne() { mBits = static_cast<T>(-1); }

    void setDirect(T bits) { mBits = bits; }
    T getDirect() const { return mBits; }
    T* getPtr() { return &mBits; }
    const T* getPtr() const { return &mBits; }
    size_t getByteSize() const { return sizeof(T); }

    void set(T mask) { mBits |= mask; }
    void reset(T mask) { mBits &= ~mask; }
    void toggle(T mask) { mBits ^= mask; }
    void change(T mask, bool b) { b ? set(mask) : reset(mask); }

    bool isZero() const { return mBits == 0; }
    bool isOn(T mask) const { return (mBits & mask) != 0; }
    bool isOnAll(T mask) const { return (mBits & mask) == mask; }
    bool isOff(T mask) const { return !isOn(mask); }

    T getMask(T mask) const { return static_cast<T>(mBits & mask); }

    static T makeMask(s32 bit)
    {
        SEAD_ASSERT(static_cast<u32>(bit) < cBitNum);
        return static_cast<T>(1 << bit);
    }

    void setBit(s32 bit) { set(makeMask(bit)); }
    void resetBit(s32 bit) { reset(makeMask(bit)); }
    void changeBit(s32 bit, bool b) { change(makeMask(bit), b); }
    void toggleBit(s32 bit) { toggle(makeMask(bit)); }

    bool isOnBit(s32 bit) const { return isOn(makeMask(bit)); }
    bool isOffBit(s32 bit) const { return isOff(makeMask(bit)); }

    s32 countOnBit() const
    {
        if constexpr (sizeof(T) <= 4)
            return BitFlagUtil::countOnBit(mBits);
        else
            return BitFlagUtil::countOnBit64(mBits);
    }

    s32 countContinuousOffBitFromRight() const
    {
        if constexpr (sizeof(T) <= 4)
            return BitFlagUtil::countContinuousOffBitFromRight(mBits);
        else
            return BitFlagUtil::countContinuousOffBitFromRight64(mBits);
    }

    s32 countRightOnBit(s32 bit) const
    {
        if constexpr (sizeof(T) <= 4)
            return BitFlagUtil::countRightOnBit(mBits, bit);
        else
            return BitFlagUtil::countRightOnBit64(mBits, bit);
    }

    s32 findOnBitFromRight(s32 num) const
    {
        if constexpr (sizeof(T) <= 4)
            return BitFlagUtil::findOnBitFromRight(mBits, num);
        else
            return BitFlagUtil::findOnBitFromRight64(mBits, num);
    }

private:
    T mBits;
};

using BitFlag8 = BitFlag<u8>;
using BitFlag16 = BitFlag<u16>;
using BitFlag32 = BitFlag<u32>;
using BitFlag64 = BitFlag<u64>;

} // namespace sead
