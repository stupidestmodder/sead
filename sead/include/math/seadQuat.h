#pragma once

#include <basis/seadTypes.h>
#include <math/seadMathPolicies.h>

namespace sead {

template <typename T>
class Vector3;

// TODO
template <typename T>
class Quat : public Policies<T>::QuatBase
{
public:
    using Self = Quat<T>;

    using Vec3 = Vector3<T>;

    static const Self unit;

public:
    Quat()
        : Policies<T>::QuatBase(0, 0, 0, 0)
    {
    }

    Quat(T w_, T x_, T y_, T z_)
        : Policies<T>::QuatBase(w_, x_, y_, z_)
    {
    }

    bool makeVectorRotation(const Vec3& from, const Vec3& to);
};

using Quatf = Quat<f32>;

} // namespace sead

#include <math/seadQuat.hpp>
