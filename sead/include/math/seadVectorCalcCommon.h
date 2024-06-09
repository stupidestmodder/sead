#pragma once

#include <math/seadMathPolicies.h>

namespace sead {

// TODO
template <typename T>
class Vector2CalcCommon
{
public:
    using Base = typename Policies<T>::Vec2Base;

public:
    static T squaredLength(const Base& v);
    static T length(const Base& v);
};

// TODO
template <typename T>
class Vector3CalcCommon
{
public:
    using Base = typename Policies<T>::Vec3Base;

public:
    static void cross(Base& o, const Base& a, const Base& b);
    static T dot(const Base& a, const Base& b);
    static T squaredLength(const Base& v);
    static T length(const Base& v);
    static void multScalar(Base& o, const Base& v, T t);
    static T normalize(Base& v);
};

} // namespace sead

#include <math/seadVectorCalcCommon.hpp>
