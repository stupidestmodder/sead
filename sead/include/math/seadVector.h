#pragma once

#include <basis/seadTypes.h>
#include <math/seadMathPolicies.h>

namespace sead {

template <typename T>
class Matrix34;

template <typename T>
class Matrix44;

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
    Vector2()
        : Policies<T>::Vec2Base(0, 0)
    {
    }

    Vector2(T x_, T y_)
        : Policies<T>::Vec2Base(x_, y_)
    {
    }

    template <typename Vec>
    Vector2(const Vec& v)
        : Policies<T>::Vec2Base(v.x, v.y)
    {
    }

    bool isZero() const
    {
        return *this == zero;
    }

    void set(T x_, T y_)
    {
        this->x = x_;
        this->y = y_;
    }

    bool operator==(const Self& v) const
    {
        return this->x == v.x && this->y == v.y;
    }

    template <typename Vec>
    Self& operator=(const Vec& v)
    {
        this->x = static_cast<T>(v.x);
        this->y = static_cast<T>(v.y);
        return *this;
    }

    Self operator+(const Self& v) const
    {
        Self o;
        o.x = this->x + v.x;
        o.y = this->y + v.y;
        return o;
    }

    Self operator*(T t) const
    {
        Self o;
        o.x = this->x * t;
        o.y = this->y * t;
        return o;
    }

    const Self& operator+=(const Self& v)
    {
        this->x += v.x;
        this->y += v.y;
        return *this;
    }

    void add(const Self& v)
    {
        this->x += v.x;
        this->y += v.y;
    }

    void div(const Self& v)
    {
        this->x /= v.x;
        this->y /= v.y;
    }

    void setSub(const Self& a, const Self& b)
    {
        this->x = a.x - b.x;
        this->y = a.y - b.y;
    }

    T length() const;
};

// TODO
template <typename T>
class Vector3 : public Policies<T>::Vec3Base
{
public:
    using Self = Vector3<T>;
    using Mtx34 = Matrix34<T>;
    using Mtx44 = Matrix44<T>;

    static const Self zero;
    static const Self ex;
    static const Self ey;
    static const Self ez;
    static const Self ones;

public:
    Vector3()
        : Policies<T>::Vec3Base(0, 0, 0)
    {
    }

    Vector3(T x_, T y_, T z_)
        : Policies<T>::Vec3Base(x_, y_, z_)
    {
    }

    void set(T x_, T y_, T z_)
    {
        this->x = x_;
        this->y = y_;
        this->z = z_;
    }

    bool operator==(const Self& v) const
    {
        return this->x == v.x && this->y == v.y && this->z == v.z;
    }

    bool operator!=(const Self& v) const
    {
        return !operator==(v);
    }

    Self operator+(const Self& v) const
    {
        Self o;
        o.x = this->x + v.x;
        o.y = this->y + v.y;
        o.z = this->z + v.z;
        return o;
    }

    Self operator-(const Self& v) const
    {
        Self o;
        o.x = this->x - v.x;
        o.y = this->y - v.y;
        o.z = this->z - v.z;
        return o;
    }

    Self operator*(T t) const
    {
        Self o;
        o.x = this->x * t;
        o.y = this->y * t;
        o.z = this->z * t;
        return o;
    }

    const Self& operator+=(const Self& v)
    {
        this->x += v.x;
        this->y += v.y;
        this->z += v.z;
        return *this;
    }

    const Self& operator-=(const Self& v)
    {
        this->x -= v.x;
        this->y -= v.y;
        this->z -= v.z;
        return *this;
    }

    void add(const Self& a);
    void multScalar(T t);
    T normalize();
    T dot(const Self&) const;
    T length() const;

    void setMul(const Mtx34& m, const Self& v);
    void setMulAndDivByW(const Mtx44& m, const Self& v);
    void setCross(const Self&, const Self&);
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
    Vector4()
        : Policies<T>::Vec4Base(0, 0, 0, 0)
    {
    }

    Vector4(T x_, T y_, T z_, T w_)
        : Policies<T>::Vec4Base(x_, y_, z_, w_)
    {
    }
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

#include <math/seadVector.hpp>
