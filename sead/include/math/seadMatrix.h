#pragma once

#include <basis/seadTypes.h>
#include <math/seadMathPolicies.h>

namespace sead {

template <typename T>
class Quat;

template <typename T>
class Vector3;

template <typename T>
class Vector4;

// TODO
template <typename T>
class Matrix22 : public Policies<T>::Mtx22Base
{
public:
    using Self = Matrix22<T>;

    static const Self zero;
    static const Self ident;

public:
    Matrix22();
    Matrix22(T, T, T, T);
};

// TODO
template <typename T>
class Matrix33 : public Policies<T>::Mtx33Base
{
public:
    using Self = Matrix33<T>;

    static const Self zero;
    static const Self ident;

public:
    Matrix33();
    Matrix33(T, T, T, T, T, T, T, T, T);
};

// TODO
template <typename T>
class Matrix34 : public Policies<T>::Mtx34Base
{
public:
    using Self = Matrix34<T>;
    using Mtx33 = Matrix33<T>;

    using Quat = Quat<T>;
    using Vec3 = Vector3<T>;

    static const Self zero;
    static const Self ident;

public:
    Matrix34()
        : Policies<T>::Mtx34Base()
    {
    }

    Matrix34(T a00, T a01, T a02, T a03, T a10, T a11, T a12, T a13, T a20, T a21, T a22, T a23)
        : Policies<T>::Mtx34Base()
    {
        this->_00 = a00;
        this->_01 = a01;
        this->_02 = a02;
        this->_03 = a03;

        this->_10 = a10;
        this->_11 = a11;
        this->_12 = a12;
        this->_13 = a13;

        this->_20 = a20;
        this->_21 = a21;
        this->_22 = a22;
        this->_23 = a23;
    }

    T operator()(s32 i, s32 j) const
    {
        return this->m[i][j];
    }

    T& operator()(s32 i, s32 j)
    {
        return this->m[i][j];
    }

    void setMul(const Self& a, const Self& b);
    void setMul(const Mtx33& a, const Self& b);

    void fromQuat(const Quat& q);
    void makeS(const Vec3& s);
    void makeS(T x, T y, T z);
    void makeSRT(const Vec3& s, const Vec3& r, const Vec3& t);
    void makeST(const Vec3& s, const Vec3& t);

    void setTranslation(const Vec3& t);
    void setTranslation(T x, T y, T z);
};

// TODO
template <typename T>
class Matrix44 : public Policies<T>::Mtx44Base
{
public:
    using Self = Matrix44<T>;
    using Mtx34 = Matrix34<T>;

    using Vec4 = Vector4<T>;

    static const Self zero;
    static const Self ident;

public:
    Matrix44()
        : Policies<T>::Mtx44Base()
    {
    }

    Matrix44(T a00, T a01, T a02, T a03, T a10, T a11, T a12, T a13, T a20, T a21, T a22, T a23, T a30, T a31, T a32, T a33)
        : Policies<T>::Mtx44Base()
    {
        this->_00 = a00;
        this->_01 = a01;
        this->_02 = a02;
        this->_03 = a03;

        this->_10 = a10;
        this->_11 = a11;
        this->_12 = a12;
        this->_13 = a13;

        this->_20 = a20;
        this->_21 = a21;
        this->_22 = a22;
        this->_23 = a23;

        this->_30 = a30;
        this->_31 = a31;
        this->_32 = a32;
        this->_33 = a33;
    }

    Matrix44(const Mtx34& mtx34, const Vec4& vw = Vec4::ew);

    T operator()(s32 i, s32 j) const
    {
        return this->m[i][j];
    }

    T& operator()(s32 i, s32 j)
    {
        return this->m[i][j];
    }

    void setMul(const Self& a, const Self& b);
    void setMul(const Mtx34& a, const Self& b);
    void setMul(const Self& a, const Mtx34& b);

    void getCol(Vec4* o, s32 axis) const;
    void getRow(Vec4* o, s32 row) const;

    void setCol(s32 axis, const Vec4& v);
    void setRow(s32 row, const Vec4& v);
};

using Matrix22f = Matrix22<f32>;

using Matrix33f = Matrix33<f32>;

using Matrix34f = Matrix34<f32>;

using Matrix44f = Matrix44<f32>;

} // namespace sead

#include <math/seadMatrix.hpp>
