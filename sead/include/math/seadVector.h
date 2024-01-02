#pragma once

#include <basis/seadTypes.h>
#include <math/seadMathPolicies.h>

namespace sead {

// TODO
template <typename T>
class Vector2 : public Policies<T>::Vec2Base
{
public:
    using Self = Vector2<T>;

    static const Self zero;
    static const Self ex;
    static const Self ey;
    static const Self ones;

public:
    Vector2();
    Vector2(T, T);
};

// TODO
template <typename T>
class Vector3 : public Policies<T>::Vec3Base
{
public:
    using Self = Vector3<T>;

    static const Self zero;
    static const Self ex;
    static const Self ey;
    static const Self ez;
    static const Self ones;

public:
    Vector3();
    Vector3(T, T, T);
};

// TODO
template <typename T>
class Vector4 : public Policies<T>::Vec4Base
{
public:
    using Self = Vector4<T>;

    static const Self zero;
    static const Self ex;
    static const Self ey;
    static const Self ez;
    static const Self ew;
    static const Self ones;

public:
    Vector4();
    Vector4(T, T, T, T);
};

using Vector2f = Vector2<f32>;
using Vector2d = Vector2<f64>;
using Vector2i = Vector2<s32>;
using Vector2u = Vector2<u32>;

using Vector3f = Vector3<f32>;
using Vector3d = Vector3<f64>;
using Vector3i = Vector3<s32>;
using Vector3u = Vector3<u32>;

using Vector4f = Vector4<f32>;
using Vector4d = Vector4<f64>;
using Vector4i = Vector4<s32>;
using Vector4u = Vector4<u32>;

} // namespace sead
