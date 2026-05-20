#pragma once

#include <math/seadMathPolicies.h>

namespace sead {

// TODO
template <typename T>
class QuatCalcCommon
{
public:
    using Base = typename Policies<T>::QuatBase;

    using Vec3 = typename Policies<T>::Vec3Base;

public:
    static void makeUnit(Base& q);
    static bool makeVectorRotation(Base& q, const Vec3& from, const Vec3& to);
    static void set(Base& q, T w, T x, T y, T z);
};

} // namespace sead

#include <math/seadQuatCalcCommon.hpp>
