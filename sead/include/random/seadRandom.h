#pragma once

#include <basis/seadAssert.h>
#include <basis/seadTypes.h>

namespace sead {

class Random
{
public:
    Random()
    {
        init();
    }

    explicit Random(u32 seed)
    {
        init(seed);
    }

    Random(u32 seed0, u32 seed1, u32 seed2, u32 seed3)
    {
        init(seed0, seed1, seed2, seed3);
    }

    void init();
    void init(u32 seed);
    void init(u32 seed0, u32 seed1, u32 seed2, u32 seed3);

    u32 getU32();

    u32 getU32(u32 ceil)
    {
        return static_cast<u32>((static_cast<u64>(getU32()) * ceil) >> 32);
    }

    u64 getU64();

    // TODO
    u64 getU64(u64 ceil);

    s32 getS32Range(s32 a, s32 b)
    {
        SEAD_ASSERT_MSG(b >= a, "b[%d] >= a[%d]", a, b);
        return getU32(b - a) + a;
    }

    // TODO
    s64 getS64Range(s64 a, s64 b);

    f32 getF32()
    {
        u32 floatBinary = (getU32() >> 9) | 0x3F800000;
        return *reinterpret_cast<f32*>(&floatBinary) - 1.0f;
    }

    f32 getF32(f32 ceil)
    {
        return getF32() * ceil;
    }

    f32 getF32Range(f32 a, f32 b)
    {
        SEAD_ASSERT_MSG(b >= a, "b[%f] >= a[%f]", a, b);
        return getF32(b - a) + a;
    }

    // TODO
    f64 getF64();

    f64 getF64(f64 ceil)
    {
        return getF64() * ceil;
    }

    f64 getF64Range(f64 a, f64 b)
    {
        SEAD_ASSERT_MSG(b >= a, "b[%f] >= a[%f]", a, b);
        return getF64(b - a) + a;
    }

    s32 getSign()
    {
        return (getU32() & 2) - 1;
    }

    bool getBool()
    {
        return static_cast<bool>(getU32() & 1);
    }

    void getContext(u32* num0, u32* num1, u32* num2, u32* num3) const;

private:
    u32 mX;
    u32 mY;
    u32 mZ;
    u32 mW;
};

} // namespace sead
