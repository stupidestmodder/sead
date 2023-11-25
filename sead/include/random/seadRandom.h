#pragma once

#include <basis/seadTypes.h>

namespace sead {

// TODO: Implement all functions
class Random
{
public:
    Random()
    {
        init();
    }

    Random(u32 seed)
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
        return getU32() * static_cast<u64>(ceil) >> 32u;
    }

    u64 getU64();
    u64 getU64(u64 ceil);
    s32 getS32Range(s32, s32);
    s64 getS64Range(s64, s64);
    f32 getF32();
    f32 getF32(f32);
    f32 getF32Range(f32, f32);
    f64 getF64();
    f64 getF64(f64);
    f64 getF64Range(f64, f64);
    s32 getSign();
    bool getBool();
    void getContext(u32* num0, u32* num1, u32* num2, u32* num3) const;

private:
    u32 mX;
    u32 mY;
    u32 mZ;
    u32 mW;
};

} // namespace sead
