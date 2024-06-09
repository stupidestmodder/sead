#pragma once

#include <math/seadMathCalcCommon.h>
#include <math/seadVectorCalcCommon.h>

namespace sead {

template <typename T>
inline T Vector2CalcCommon<T>::squaredLength(const Base& v)
{
    return v.x * v.x + v.y * v.y;
}

template <typename T>
inline T Vector2CalcCommon<T>::length(const Base& v)
{
    return MathCalcCommon<T>::sqrt(squaredLength(v));
}

template <typename T>
inline void Vector3CalcCommon<T>::cross(Base& o, const Base& a, const Base& b)
{
    Base v;

    v.x = a.y * b.z - a.z * b.y;
    v.y = a.z * b.x - a.x * b.z;
    v.z = a.x * b.y - a.y * b.x;

    o.x = v.x;
    o.y = v.y;
    o.z = v.z;
}

template <typename T>
inline T Vector3CalcCommon<T>::dot(const Base& a, const Base& b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

template <typename T>
inline T Vector3CalcCommon<T>::squaredLength(const Base& v)
{
    return v.x * v.x + v.y * v.y + v.z * v.z;
}

template <typename T>
inline T Vector3CalcCommon<T>::length(const Base& v)
{
    return MathCalcCommon<T>::sqrt(squaredLength(v));
}

template <typename T>
inline void Vector3CalcCommon<T>::multScalar(Base& o, const Base& v, T t)
{
    o.x = v.x * t;
    o.y = v.y * t;
    o.z = v.z * t;
}

template <typename T>
T Vector3CalcCommon<T>::normalize(Base& v)
{
    const T len = length(v);
    if (len > 0)
    {
        const T invLen = 1 / len;
        v.x *= invLen;
        v.y *= invLen;
        v.z *= invLen;
    }

    return len;
}

} // namespace sead
