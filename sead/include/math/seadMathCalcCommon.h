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

    static const SinCosSample cSinCosTbl[256 + 1];
    static const AtanSample cAtanTbl[128 + 1];
    static const ExpSample cExpTbl[32 + 1];
    static const LogSample cLogTbl[256 + 1];

public:
    static T piHalf() { return 1.570796326794896619231; }
    static T pi()     { return 3.141592653589793238462; }
    static T pi2()    { return 6.283185307179586476924; }
    static T zero()   { return 0; }
    static T one()    { return 1; }
    static T ln2()    { return 0.69314718055994530942; }
    static T ln2Inv() { return 1.442695040888963407; }

    static T neg(T);
    static T inv(T);
    static T sign(T);
    static T fitSign(T, T);
    static T square(T);
    static T sqrt(T);
    static T rsqrt(T);
    static T pow(T, T);
    static T powTable(T, T);
    static T sin(T);
    static T cos(T);
    static T tan(T);
    static T asin(T);
    static T acos(T);
    static T atan(T);
    static T atan2(T, T);
    static T sinIdx(u32);
    static T cosIdx(u32);
    static T tanIdx(u32);
    static u32 asinIdx(T);
    static u32 acosIdx(T);
    static u32 atanIdx(T);
    static u32 atan2Idx(T, T);
    static void sinCosIdx(T*, T*, u32);
    static T exp(T);
    static T log(T);
    static T log2(T);
    static T log10(T);
    static T expTable(T);
    static T logTable(T);
    static T minNumber();
    static T maxNumber();
    static T infinity();
    static T nan();
    static T epsilon();
    static bool equalsEpsilon(T, T, T);
    static T abs(T);
    static T max(T, T);
    static T min(T, T);
    static T max3(T, T, T);
    static T min3(T, T, T);
    static T deg2rad(T);
    static T rad2deg(T);
    static u32 deg2idx(T);
    static u32 rad2idx(T);
    static T idx2deg(u32);
    static T idx2rad(u32);
    static T roundAngle(T);
    static T angleDist(T, T);
    static T random();
    static T getRand(T);
    static T getRandRange(T, T);
    static T getRandSign();
    static s32 roundOff(T);
    static s32 floor(T);
    static s32 ceil(T);
    static T roundUpN(T, s32);
    static T roundUpPow2(T, s32);
    static T roundDownN(T, s32);
    static T roundDownPow2(T, s32);
    static T clampMax(T, T);
    static T clampMin(T, T);
    static T clamp2(T, T, T);
    static T gcd(T, T);
    static T lcm(T, T);
    static bool isZero(T, T);
    static bool isNan(T);
    static bool isPow2(T);
    static bool isMultiplePow2(T, T);
    static bool isInfinity(T);
    static bool isIntersect1d(T, T, T, T);
    static bool chase(T*, T, const T);
    static bool chaseAngle(T*, T, const T);
    static bool chaseAngleIdx(u32*, u32, s64);

protected:
    static u32 atanIdx_(T);
    static T expLn2_(T);
    static T log1_2_(T);
};

using Mathi = MathCalcCommon<s32>;
using Mathu = MathCalcCommon<u32>;
using Mathf = MathCalcCommon<f32>;
using Mathd = MathCalcCommon<f64>;

} // namespace sead

#include <math/seadMathCalcCommon.hpp>
