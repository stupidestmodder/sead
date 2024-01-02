#pragma once

#include <basis/seadTypes.h>
#include <math/seadMathPolicies.h>

namespace sead {

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

    static const Self zero;
    static const Self ident;

public:
    Matrix34();
    Matrix34(T, T, T, T, T, T, T, T, T, T, T, T);
};

// TODO
template <typename T>
class Matrix44 : public Policies<T>::Mtx44Base
{
public:
    using Self = Matrix44<T>;

    static const Self zero;
    static const Self ident;

public:
    Matrix44();
    Matrix44(T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T);
};

using Matrix22f = Matrix22<f32>;

using Matrix33f = Matrix33<f32>;

using Matrix34f = Matrix34<f32>;

using Matrix44f = Matrix44<f32>;

} // namespace sead
