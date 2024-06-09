#pragma once

#include <math/seadVector.h>
#include <math/seadVectorCalcCommon.h>

namespace sead {

template <typename T>
inline T Vector2<T>::length() const
{
    return Vector2CalcCommon<T>::length(*this);
}

template <typename T>
inline void Vector3<T>::multScalar(T t)
{
    Vector3CalcCommon<T>::multScalar(*this, *this, t);
}

template <typename T>
inline T Vector3<T>::normalize()
{
    return Vector3CalcCommon<T>::normalize(*this);
}

template <typename T>
inline T Vector3<T>::dot(const Self& t) const
{
    return Vector3CalcCommon<T>::dot(*this, t);
}

template <typename T>
inline T Vector3<T>::length() const
{
    return Vector3CalcCommon<T>::length(*this);
}

template <typename T>
inline void Vector3<T>::setCross(const Self& a, const Self& b)
{
    Vector3CalcCommon<T>::cross(*this, a, b);
}

} // namespace sead
