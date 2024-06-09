#pragma once

#include <math/seadMathCalcCommon.h>
#include <math/seadMatrixCalcCommon.h>

namespace sead {

template <typename T>
void Matrix34CalcCommon<T>::multiply(Base& o, const Base& a, const Base& b)
{
    const T a11 = a.m[0][0];
    const T a12 = a.m[0][1];
    const T a13 = a.m[0][2];
    const T a14 = a.m[0][3];

    const T a21 = a.m[1][0];
    const T a22 = a.m[1][1];
    const T a23 = a.m[1][2];
    const T a24 = a.m[1][3];

    const T a31 = a.m[2][0];
    const T a32 = a.m[2][1];
    const T a33 = a.m[2][2];
    const T a34 = a.m[2][3];

    const T b11 = b.m[0][0];
    const T b12 = b.m[0][1];
    const T b13 = b.m[0][2];
    const T b14 = b.m[0][3];

    const T b21 = b.m[1][0];
    const T b22 = b.m[1][1];
    const T b23 = b.m[1][2];
    const T b24 = b.m[1][3];

    const T b31 = b.m[2][0];
    const T b32 = b.m[2][1];
    const T b33 = b.m[2][2];
    const T b34 = b.m[2][3];

    o.m[0][0] = a11 * b11 + a12 * b21 + a13 * b31;
    o.m[0][1] = a11 * b12 + a12 * b22 + a13 * b32;
    o.m[0][2] = a11 * b13 + a12 * b23 + a13 * b33;
    o.m[0][3] = a11 * b14 + a12 * b24 + a13 * b34 + a14;

    o.m[1][0] = a21 * b11 + a22 * b21 + a23 * b31;
    o.m[1][1] = a21 * b12 + a22 * b22 + a23 * b32;
    o.m[1][2] = a21 * b13 + a22 * b23 + a23 * b33;
    o.m[1][3] = a21 * b14 + a22 * b24 + a23 * b34 + a24;

    o.m[2][0] = a31 * b11 + a32 * b21 + a33 * b31;
    o.m[2][1] = a31 * b12 + a32 * b22 + a33 * b32;
    o.m[2][2] = a31 * b13 + a32 * b23 + a33 * b33;
    o.m[2][3] = a31 * b14 + a32 * b24 + a33 * b34 + a34;
}

template <typename T>
void Matrix34CalcCommon<T>::makeQ(Base& o, const Quat& q)
{
    // Assuming the quaternion "q" is normalized

    const T yy = 2 * q.y * q.y;
    const T zz = 2 * q.z * q.z;
    const T xx = 2 * q.x * q.x;
    const T xy = 2 * q.x * q.y;
    const T xz = 2 * q.x * q.z;
    const T yz = 2 * q.y * q.z;
    const T wz = 2 * q.w * q.z;
    const T wx = 2 * q.w * q.x;
    const T wy = 2 * q.w * q.y;

    o.m[0][0] = 1 - yy - zz;
    o.m[0][1] =     xy - wz;
    o.m[0][2] =     xz + wy;

    o.m[1][0] =     xy + wz;
    o.m[1][1] = 1 - xx - zz;
    o.m[1][2] =     yz - wx;

    o.m[2][0] =     xz - wy;
    o.m[2][1] =     yz + wx;
    o.m[2][2] = 1 - xx - yy;

    o.m[0][3] = 0;
    o.m[1][3] = 0;
    o.m[2][3] = 0;
}

template <typename T>
void Matrix34CalcCommon<T>::makeS(Base& o, const Vec3& s)
{
    o.m[0][0] = s.x;
    o.m[1][0] = 0;
    o.m[2][0] = 0;

    o.m[0][1] = 0;
    o.m[1][1] = s.y;
    o.m[2][1] = 0;

    o.m[0][2] = 0;
    o.m[1][2] = 0;
    o.m[2][2] = s.z;

    o.m[0][3] = 0;
    o.m[1][3] = 0;
    o.m[2][3] = 0;
}

template <typename T>
void Matrix34CalcCommon<T>::makeSRT(Base& o, const Vec3& s, const Vec3& r, const Vec3& t)
{
    const T sinV[3] = { MathCalcCommon<T>::sin(r.x),
                        MathCalcCommon<T>::sin(r.y),
                        MathCalcCommon<T>::sin(r.z) };

    const T cosV[3] = { MathCalcCommon<T>::cos(r.x),
                        MathCalcCommon<T>::cos(r.y),
                        MathCalcCommon<T>::cos(r.z) };

    o.m[0][0] = s.x * (cosV[1] * cosV[2]);
    o.m[1][0] = s.x * (cosV[1] * sinV[2]);
    o.m[2][0] = s.x * -sinV[1];

    o.m[0][1] = s.y * (sinV[0] * sinV[1] * cosV[2] - cosV[0] * sinV[2]);
    o.m[1][1] = s.y * (sinV[0] * sinV[1] * sinV[2] + cosV[0] * cosV[2]);
    o.m[2][1] = s.y * (sinV[0] * cosV[1]);

    o.m[0][2] = s.z * (cosV[0] * cosV[2] * sinV[1] + sinV[0] * sinV[2]);
    o.m[1][2] = s.z * (cosV[0] * sinV[2] * sinV[1] - sinV[0] * cosV[2]);
    o.m[2][2] = s.z * (cosV[0] * cosV[1]);

    o.m[0][3] = t.x;
    o.m[1][3] = t.y;
    o.m[2][3] = t.z;
}

template <typename T>
void Matrix34CalcCommon<T>::makeST(Base& o, const Vec3& s, const Vec3& t)
{
    o.m[0][0] = s.x;
    o.m[1][0] = 0;
    o.m[2][0] = 0;

    o.m[0][1] = 0;
    o.m[1][1] = s.y;
    o.m[2][1] = 0;

    o.m[0][2] = 0;
    o.m[1][2] = 0;
    o.m[2][2] = s.z;

    o.m[0][3] = t.x;
    o.m[1][3] = t.y;
    o.m[2][3] = t.z;
}

template <typename T>
void Matrix44CalcCommon<T>::copy(Base& o, const Mtx34& n, const Vec4& v)
{
    o.m[0][0] = n.m[0][0];
    o.m[0][1] = n.m[0][1];
    o.m[0][2] = n.m[0][2];
    o.m[0][3] = n.m[0][3];

    o.m[1][0] = n.m[1][0];
    o.m[1][1] = n.m[1][1];
    o.m[1][2] = n.m[1][2];
    o.m[1][3] = n.m[1][3];

    o.m[2][0] = n.m[2][0];
    o.m[2][1] = n.m[2][1];
    o.m[2][2] = n.m[2][2];
    o.m[2][3] = n.m[2][3];

    o.m[3][0] = v.x;
    o.m[3][1] = v.y;
    o.m[3][2] = v.z;
    o.m[3][3] = v.w;
}

template <typename T>
void Matrix44CalcCommon<T>::multiply(Base& o, const Base& a, const Mtx34& b)
{
    const T a11 = a.m[0][0];
    const T a12 = a.m[0][1];
    const T a13 = a.m[0][2];
    const T a14 = a.m[0][3];

    const T a21 = a.m[1][0];
    const T a22 = a.m[1][1];
    const T a23 = a.m[1][2];
    const T a24 = a.m[1][3];

    const T a31 = a.m[2][0];
    const T a32 = a.m[2][1];
    const T a33 = a.m[2][2];
    const T a34 = a.m[2][3];

    const T a41 = a.m[3][0];
    const T a42 = a.m[3][1];
    const T a43 = a.m[3][2];
    const T a44 = a.m[3][3];

    const T b11 = b.m[0][0];
    const T b12 = b.m[0][1];
    const T b13 = b.m[0][2];
    const T b14 = b.m[0][3];

    const T b21 = b.m[1][0];
    const T b22 = b.m[1][1];
    const T b23 = b.m[1][2];
    const T b24 = b.m[1][3];

    const T b31 = b.m[2][0];
    const T b32 = b.m[2][1];
    const T b33 = b.m[2][2];
    const T b34 = b.m[2][3];

    o.m[0][0] = a11 * b11 + a12 * b21 + a13 * b31;
    o.m[0][1] = a11 * b12 + a12 * b22 + a13 * b32;
    o.m[0][2] = a11 * b13 + a12 * b23 + a13 * b33;
    o.m[0][3] = a11 * b14 + a12 * b24 + a13 * b34 + a14;

    o.m[1][0] = a21 * b11 + a22 * b21 + a23 * b31;
    o.m[1][1] = a21 * b12 + a22 * b22 + a23 * b32;
    o.m[1][2] = a21 * b13 + a22 * b23 + a23 * b33;
    o.m[1][3] = a21 * b14 + a22 * b24 + a23 * b34 + a24;

    o.m[2][0] = a31 * b11 + a32 * b21 + a33 * b31;
    o.m[2][1] = a31 * b12 + a32 * b22 + a33 * b32;
    o.m[2][2] = a31 * b13 + a32 * b23 + a33 * b33;
    o.m[2][3] = a31 * b14 + a32 * b24 + a33 * b34 + a34;

    o.m[3][0] = a41 * b11 + a42 * b21 + a43 * b31;
    o.m[3][1] = a41 * b12 + a42 * b22 + a43 * b32;
    o.m[3][2] = a41 * b13 + a42 * b23 + a43 * b33;
    o.m[3][3] = a41 * b14 + a42 * b24 + a43 * b34 + a44;
}

template <typename T>
void Matrix34CalcCommon<T>::setBase(Base& n, s32 axis, const Vec3& v)
{
    n.m[0][axis] = v.x;
    n.m[1][axis] = v.y;
    n.m[2][axis] = v.z;
}

template <typename T>
void Matrix34CalcCommon<T>::setTranslation(Base& n, const Vec3& v)
{
    setBase(n, 3, v);
}

template <typename T>
void Matrix44CalcCommon<T>::getRow(Vec4* v, const Base& n, s32 row)
{
    v->x = n.m[row][0];
    v->y = n.m[row][1];
    v->z = n.m[row][2];
    v->w = n.m[row][3];
}

template <typename T>
void Matrix44CalcCommon<T>::setRow(Base& n, const Vec4& v, s32 row)
{
    n.m[row][0] = v.x;
    n.m[row][1] = v.y;
    n.m[row][2] = v.z;
    n.m[row][3] = v.w;
}

} // namespace sead
