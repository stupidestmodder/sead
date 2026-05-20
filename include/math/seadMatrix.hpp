#pragma once

#include <math/seadMatrix.h>
#include <math/seadMatrixCalcCommon.h>

namespace sead {

template <typename T>
inline void Matrix34<T>::setMul(const Self& a, const Self& b)
{
    Matrix34CalcCommon<T>::multiply(*this, a, b);
}

template <typename T>
inline void Matrix34<T>::fromQuat(const Quat& q)
{
    Matrix34CalcCommon<T>::makeQ(*this, q);
}

template <typename T>
inline void Matrix34<T>::makeS(const Vec3& s)
{
    Matrix34CalcCommon<T>::makeS(*this, s);
}

template <typename T>
inline void Matrix34<T>::makeS(T x, T y, T z)
{
    Vec3 s(x, y, z);
    Matrix34CalcCommon<T>::makeS(*this, s);
}

template <typename T>
inline void Matrix34<T>::makeSRT(const Vec3& s, const Vec3& r, const Vec3& t)
{
    Matrix34CalcCommon<T>::makeSRT(*this, s, r, t);
}

template <typename T>
inline void Matrix34<T>::makeST(const Vec3& s, const Vec3& t)
{
    Matrix34CalcCommon<T>::makeST(*this, s, t);
}

template <typename T>
inline void Matrix34<T>::setTranslation(const Vec3& t)
{
    Matrix34CalcCommon<T>::setTranslation(*this, t);
}

template <typename T>
inline void
Matrix34<T>::setTranslation(T x, T y, T z)
{
    Vec3 t(x, y, z);
    Matrix34CalcCommon<T>::setTranslation(*this, t);
}

template <typename T>
inline Matrix44<T>::Matrix44(const Mtx34& mtx34, const Vec4& vw)
{
    Matrix44CalcCommon<T>::copy(*this, mtx34, vw);
}

template <typename T>
inline void Matrix44<T>::setMul(const Self& a, const Mtx34& b)
{
    Matrix44CalcCommon<T>::multiply(*this, a, b);
}

template <typename T>
inline void Matrix44<T>::getRow(Vec4* o, s32 row) const
{
    Matrix44CalcCommon<T>::getRow(o, *this, row);
}

template <typename T>
inline void Matrix44<T>::setRow(s32 row, const Vec4& v)
{
    Matrix44CalcCommon<T>::setRow(*this, v, row);
}

} // namespace sead
