#pragma once

#include <basis/seadAssert.h>
#include <math/seadMathCalcCommon.h>

#include <cmath>
#include <limits>

namespace sead {

template <typename T>
inline T MathCalcCommon<T>::sqrt(T t)
{
    return std::sqrt(t);
}

template <typename T>
inline T MathCalcCommon<T>::sin(T t)
{
    return std::sin(t);
}

template <typename T>
inline T MathCalcCommon<T>::cos(T t)
{
    return std::cos(t);
}

template <typename T>
inline T MathCalcCommon<T>::tan(T t)
{
    return std::tan(t);
}

template <typename T>
inline T MathCalcCommon<T>::atan2(T y, T x)
{
    return std::atan2(y, x);
}

template <>
inline f32 MathCalcCommon<f32>::sinIdx(u32 idx)
{
    u32 index = (idx >> 24) & 0xFF;
    u32 rest = idx & 0xFFFFFF;

    return cSinCosTbl[index].sin_val + cSinCosTbl[index].sin_delta * rest / 0x1000000;
}

template <>
inline u32 MathCalcCommon<f32>::atan2Idx(f32 y, f32 x)
{
    if (x == 0 && y == 0)
        return 0;

    if (x >= 0)
    {
        if (y >= 0)
        {
            if (x >= y)
                return atanIdx_(y / x);

            else
                return 0x40000000 - atanIdx_(x / y);
        }
        else
        {
            if (x >= -y)
                return ~(atanIdx_(-y / x) - 1);

            else
                return 0xC0000000 + atanIdx_(x / -y);
        }
    }
    else
    {
        if (y >= 0)
        {
            if (-x >= y)
                return 0x80000000 - atanIdx_(y / -x);

            else
                return 0x40000000 + atanIdx_(-x / y);
        }
        else
        {
            if (x <= y)
                return 0x80000000 + atanIdx_(y / x);

            else
                return 0xC0000000 - atanIdx_(x / y);
        }
    }
}

template <typename T>
inline T MathCalcCommon<T>::minNumber()
{
    return std::numeric_limits<T>::min();
}

template <typename T>
inline T MathCalcCommon<T>::maxNumber()
{
    return std::numeric_limits<T>::max();
}

template <>
inline f32 MathCalcCommon<f32>::minNumber()
{
    return -std::numeric_limits<f32>::max();
}

template <>
inline f32 MathCalcCommon<f32>::maxNumber()
{
    return std::numeric_limits<f32>::max();
}

template <>
inline f64 MathCalcCommon<f64>::minNumber()
{
    return -std::numeric_limits<f64>::max();
}

template <>
inline f64 MathCalcCommon<f64>::maxNumber()
{
    return std::numeric_limits<f64>::max();
}

template <typename T>
inline T MathCalcCommon<T>::epsilon()
{
    return std::numeric_limits<T>::epsilon();
}

template <>
inline s32 MathCalcCommon<s32>::abs(s32 t)
{
    return (t ^ t >> 31) - (t >> 31);
}

template <>
inline s32 MathCalcCommon<s32>::roundUpN(s32 val, s32 multNumber)
{
    SEAD_ASSERT(multNumber > 0);
    return (val >= 0 ? (val + multNumber - 1) / multNumber : (val - multNumber + 1) / multNumber) * multNumber;
}

template <>
inline u16 MathCalcCommon<u16>::roundUpPow2(u16 val, s32 base)
{
    SEAD_ASSERT_MSG(base > 0 && (base - 1u & base) == 0, "illegal param[base:%d]", base);
    return val + (base - 1u) & ~(base - 1u);
}

template <>
inline s32 MathCalcCommon<s32>::roundUpPow2(s32 val, s32 base)
{
    SEAD_ASSERT_MSG(val >= 0 && base > 0 && (base - 1u & base) == 0, "illegal param[val:%d, base:%d]", val, base);
    return val + (base - 1u) & ~(base - 1u);
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

template <typename T>
inline T MathCalcCommon<T>::clampMax(T val, T max_)
{
    if (val > max_)
        return max_;

    return val;
}

template <typename T>
inline T MathCalcCommon<T>::clampMin(T val, T min_)
{
    if (val < min_)
        return min_;

    return val;
}

template <typename T>
inline T MathCalcCommon<T>::clamp2(T min_, T val, T max_)
{
    if (val < min_)
        val = min_;
    else if (val > max_)
        val = max_;

    return val;
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
