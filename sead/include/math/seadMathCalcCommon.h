#pragma once

#include <basis/seadTypes.h>

namespace sead {

// TODO
template <typename T>
class MathCalcCommon
{
public:
    static const u32 cQuarterRoundIdx = 0x40000000; //  90 degrees, PI/2 radians
    static const u32 cHalfRoundIdx    = 0x80000000; // 180 degrees, PI   radians

    struct SinCosSample
    {
        T sin_val;
        T sin_delta;
        T cos_val;
        T cos_delta;
    };

    struct AtanSample
    {
        u32 atan_val;
        T atan_delta;
    };

    struct ExpSample
    {
        T exp_val;
        T exp_delta;
    };

    struct LogSample
    {
        T log_val;
        T log_delta;
    };

public:
    static T piHalf() { return 1.570796326794896619231; }
    static T pi()     { return 3.141592653589793238462; }
    static T pi2()    { return 6.283185307179586476924; }
    static T zero()   { return 0; }
    static T one()    { return 1; }
    static T ln2()    { return 0.69314718055994530942; }
    static T ln2Inv() { return 1.442695040888963407; }

    static T abs(T);
};

using Mathi = MathCalcCommon<s32>;
using Mathu = MathCalcCommon<u32>;
using Mathf = MathCalcCommon<f32>;
using Mathd = MathCalcCommon<f64>;

} // namespace sead

#include <math/seadMathCalcCommon.hpp>
