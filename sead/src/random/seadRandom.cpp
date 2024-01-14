#include <random/seadRandom.h>

#include <basis/seadAssert.h>
#include <time/seadTickTime.h>

namespace sead {

void Random::init()
{
    init(static_cast<u32>(TickTime().toU64()));
}

void Random::init(u32 seed)
{
    static const u32 a = 1812433253;

    mX = (seed ^ (seed >> 30)) * a + 1;
    mY = (mX   ^ (mX   >> 30)) * a + 2;
    mZ = (mY   ^ (mY   >> 30)) * a + 3;
    mW = (mZ   ^ (mZ   >> 30)) * a + 4;
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
    u32 t = mX ^ (mX << 11);

    mX = mY;
    mY = mZ;
    mZ = mW;
    mW = (mW ^ (mW >> 19)) ^ (t ^ (t >> 8));

    return mW;
}

u64 Random::getU64()
{
    return (static_cast<u64>(getU32()) << 32) | getU32();
}

void Random::getContext(u32* num0, u32* num1, u32* num2, u32* num3) const
{
    *num0 = mX;
    *num1 = mY;
    *num2 = mZ;
    *num3 = mW;
}

} // namespace sead
