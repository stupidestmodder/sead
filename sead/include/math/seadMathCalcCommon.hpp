#pragma once

#include <basis/seadAssert.h>
#include <math/seadMathCalcCommon.h>

namespace sead {

template <>
inline s32 MathCalcCommon<s32>::abs(s32 t)
{
    return (t ^ t >> 31) - (t >> 31);
}

template <>
inline u32 MathCalcCommon<u32>::roundUpPow2(u32 val, s32 base)
{
    SEAD_ASSERT_MSG(base > 0 && (base - 1u & base) == 0, "illegal param[base:%d]", base);
    return val + (base - 1u) & ~(base - 1u);
}

template <>
inline u64 MathCalcCommon<u64>::roundUpPow2(u64 val, s32 base)
{
    SEAD_ASSERT_MSG(base > 0 && (base - 1u & base) == 0, "illegal param[base:%d]", base);
    return val + (base - 1llu) & ~(base - 1llu);
}

template <>
inline u32 MathCalcCommon<u32>::roundDownPow2(u32 val, s32 base)
{
    SEAD_ASSERT_MSG(base > 0 && (base - 1u & base) == 0, "illegal param[base:%d]", base);
    return val & ~(base - 1u);
}

template <>
inline u64 MathCalcCommon<u64>::roundDownPow2(u64 val, s32 base)
{
    SEAD_ASSERT_MSG(base > 0 && (base - 1u & base) == 0, "illegal param[base:%d]", base);
    return val & ~(base - 1llu);
}

template <>
inline bool MathCalcCommon<s32>::isPow2(s32 t)
{
    return (t & t - 1u) == 0;
}

template <>
inline bool MathCalcCommon<u32>::isMultiplePow2(u32 n, u32 mult)
{
    return (n & mult - 1) == 0;
}

template <>
inline bool MathCalcCommon<u64>::isMultiplePow2(u64 n, u64 mult)
{
    return (n & mult - 1) == 0;
}

} // namespace sead
