#include <random/seadRandom.h>

#include <basis/seadAssert.h>
#include <time/seadTickTime.h>

namespace sead {

void Random::init()
{
    TickTime now;
    init(static_cast<u32>(now.toU64()));
}

void Random::init(u32 seed)
{
    const u32 constant = 0x6C078965;

    mX = constant * (seed ^ (seed >> 30u)) + 1;
    mY = constant * (mX   ^ (mX   >> 30u)) + 2;
    mZ = constant * (mY   ^ (mY   >> 30u)) + 3;
    mW = constant * (mZ   ^ (mZ   >> 30u)) + 4;
}

void Random::init(u32 seed0, u32 seed1, u32 seed2, u32 seed3)
{
    if (seed0 == 0 && seed1 == 0 && seed2 == 0 && seed3 == 0)
    {
        SEAD_ASSERT_MSG(false, "seeds must not be all zero.");
        init(0);
    }
    else
    {
        mX = seed0;
        mY = seed1;
        mZ = seed2;
        mW = seed3;
    }
}

u32 Random::getU32()
{
    u32 x = mX ^ (mX << 11u);

    mX = mY;
    mY = mZ;
    mZ = mW;
    mW = mW ^ (mW >> 19u) ^ x ^ (x >> 8u);

    return mW;
}

u64 Random::getU64()
{
    return u64(getU32()) << 32u | getU32();
}

void Random::getContext(u32* num0, u32* num1, u32* num2, u32* num3) const
{
    *num0 = mX;
    *num1 = mY;
    *num2 = mZ;
    *num3 = mW;
}

} // namespace sead
