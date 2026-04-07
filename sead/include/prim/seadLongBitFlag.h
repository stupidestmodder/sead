#pragma once

#include <basis/seadAssert.h>

namespace sead {

template <s32 N>
class LongBitFlag
{
public:
    enum
    {
        cBitNum = N
    };

public:
    LongBitFlag()
    {
        makeAllZero();
    }

    void makeAllZero()
    {
        for (s32 i = 0; i < sizeof(mBits) / sizeof(u32); i++)
        {
            mBits[i] = 0;
        }
    }

    void makeAllOne()
    {
        for (s32 i = 0; i < sizeof(mBits) / sizeof(u32); i++)
        {
            mBits[i] = static_cast<u32>(-1);
        }
    }

    size_t getByteSize() const { return sizeof(mBits); }

    void setBit(s32 bit)
    {
        if (static_cast<u32>(bit) >= cBitNum)
        {
            SEAD_ASSERT_MSG(false, "range over [0,%d) : %d", cBitNum, bit);
            return;
        }

        mBits[bit >> 5] |= 1 << (bit & 31);
    }

    void resetBit(s32 bit)
    {
        if (static_cast<u32>(bit) >= cBitNum)
        {
            SEAD_ASSERT_MSG(false, "range over [0,%d) : %d", cBitNum, bit);
            return;
        }

        mBits[bit >> 5] &= ~(1 << (bit & 31));
    }

    void changeBit(s32 bit, bool b)
    {
        if (b)
        {
            setBit(bit);
        }
        else
        {
            resetBit(bit);
        }
    }

    void toggleBit(s32 bit)
    {
        if (static_cast<u32>(bit) >= cBitNum)
        {
            SEAD_ASSERT_MSG(false, "range over [0,%d) : %d", cBitNum, bit);
            return;
        }

        mBits[bit >> 5] ^= 1 << (bit & 31);
    }

    bool isZero() const
    {
        for (s32 i = 0; i < sizeof(mBits) / sizeof(u32); i++)
        {
            if (mBits[i] != 0)
            {
                return false;
            }
        }

        return true;
    }

    bool isOnBit(s32 bit) const
    {
        SEAD_ASSERT_MSG(static_cast<u32>(bit) < cBitNum, "range over [0,%d) : %d", cBitNum, bit);
        return (mBits[bit >> 5] & 1 << (bit & 31)) != 0;
    }

    bool isOffBit(s32 bit) const
    {
        return !isOnBit(bit);
    }

    s32 countOnBit() const
    {
        s32 count = 0;
        for (s32 i = 0; i < sizeof(mBits) / sizeof(u32); i++)
        {
            u32 bits = mBits[i];
            while (bits)
            {
                count += bits & 1;
                bits >>= 1;
            }
        }

        return count;
    }

    s32 countRightOnBit(s32 bit) const
    {
        s32 count = 0;
        s32 index = bit >> 5;
        for (s32 i = 0; i < index; i++)
        {
            u32 bits = mBits[i];
            while (bits)
            {
                count += bits & 1;
                bits >>= 1;
            }
        }

        u32 bits = mBits[index] & ((1 << ((bit & 31) + 1)) - 1);
        while (bits)
        {
            count += bits & 1;
            bits >>= 1;
        }

        return count;
    }

protected:
    u32 mBits[(N + 31) / 32];
};

} // namespace sead
