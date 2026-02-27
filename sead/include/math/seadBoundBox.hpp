#pragma once

#include <math/seadBoundBox.h>

namespace sead {

template <typename T>
inline bool BoundBox2<T>::isUndef() const
{
    return mMin.x > mMax.x || mMin.y > mMax.y;
}

template <typename T>
inline void BoundBox2<T>::set(T x0, T y0, T x1, T y1)
{
    if (x0 < x1)
    {
        mMin.x = x0;
        mMax.x = x1;
    }
    else
    {
        mMin.x = x1;
        mMax.x = x0;
    }

    if (y0 < y1)
    {
        mMin.y = y0;
        mMax.y = y1;
    }
    else
    {
        mMin.y = y1;
        mMax.y = y0;
    }
}

template <typename T>
inline void BoundBox2<T>::set(const Vector2& min, const Vector2& max)
{
    mMin = min;
    mMax = max;
}

template <typename T>
inline void BoundBox2<T>::setUndef()
{
    mMin.set(MathCalcCommon<T>::maxNumber(), MathCalcCommon<T>::maxNumber());
    mMax.set(MathCalcCommon<T>::minNumber(), MathCalcCommon<T>::minNumber());
}

template <typename T>
inline void BoundBox2<T>::setFromCornerAndXY(T cornerX, T cornerY, T sizeX, T sizeY)
{
    mMin.set(cornerX, cornerY);
    mMax.set(cornerX + sizeX, cornerY + sizeY);
}

template <typename T>
inline void BoundBox2<T>::setFromCornerAndXY(const Vector2& corner, T sizeX, T sizeY)
{
    setFromCornerAndXY(corner.x, corner.y, sizeX, sizeY);
}

template <typename T>
inline void BoundBox2<T>::setFromCenterAndXY(T centerX, T centerY, T sizeX, T sizeY)
{
    T halfWidth = sizeX / 2.0f;
    T halfHeight = sizeY / 2.0f;

    mMin.set(centerX - halfWidth, centerY - halfHeight);
    mMax.set(centerX + halfWidth, centerY + halfHeight);
}

template <typename T>
inline void BoundBox2<T>::setFromCenterAndXY(const Vector2& center, T sizeX, T sizeY)
{
    setFromCenterAndXY(center.x, center.y, sizeX, sizeY);
}

template <typename T>
Vector2<T> BoundBox2<T>::getCenter() const
{
    return Vector2((mMin.x + mMax.x) / 2.0f, (mMin.y + mMax.y) / 2.0f);
}

template <typename T>
void BoundBox2<T>::getCenter(Vector2* p) const
{
    p->set((mMin.x + mMax.x) / 2.0f, (mMin.y + mMax.y) / 2.0f);
}

template <typename T>
bool BoundBox2<T>::isInside(const Vector2& p) const
{
    return mMin.x <= p.x && p.x <= mMax.x &&
           mMin.y <= p.y && p.y <= mMax.y;
}

template <typename T>
inline void BoundBox2<T>::offset(T dx, T dy)
{
    mMin.x += dx;
    mMin.y += dy;
    mMax.x += dx;
    mMax.y += dy;
}

template <typename T>
inline void BoundBox2<T>::offset(const Vector2& dv)
{
    offset(dv.x, dv.y);
}

template <typename T>
inline void BoundBox2<T>::scaleX(T sx)
{
    T hx = getSizeX() * (sx / 2.0f);
    T ox = (mMin.x + mMax.x) / 2.0f;

    mMin.x = ox - hx;
    mMax.x = ox + hx;
}

template <typename T>
inline void BoundBox2<T>::scaleY(T sy)
{
    T hy = getSizeY() * (sy / 2.0f);
    T oy = (mMin.y + mMax.y) / 2.0f;

    mMin.y = oy - hy;
    mMax.y = oy + hy;
}

template <typename T>
inline bool BoundBox3<T>::isUndef() const
{
    return mMin.x > mMax.x || mMin.y > mMax.y || mMin.z > mMax.z;
}

template <typename T>
inline void BoundBox3<T>::set(T x0, T y0, T z0, T x1, T y1, T z1)
{
    if (x0 < x1)
    {
        mMin.x = x0;
        mMax.x = x1;
    }
    else
    {
        mMin.x = x1;
        mMax.x = x0;
    }

    if (y0 < y1)
    {
        mMin.y = y0;
        mMax.y = y1;
    }
    else
    {
        mMin.y = y1;
        mMax.y = y0;
    }

    if (z0 < z1)
    {
        mMin.z = z0;
        mMax.z = z1;
    }
    else
    {
        mMin.z = z1;
        mMax.z = z0;
    }
}

template <typename T>
inline void BoundBox3<T>::set(const Vector3& min, const Vector3& max)
{
    mMin = min;
    mMax = max;
}

template <typename T>
inline void BoundBox3<T>::setUndef()
{
    mMin.set(MathCalcCommon<T>::maxNumber(), MathCalcCommon<T>::maxNumber(), MathCalcCommon<T>::maxNumber());
    mMax.set(MathCalcCommon<T>::minNumber(), MathCalcCommon<T>::minNumber(), MathCalcCommon<T>::minNumber());
}

template <typename T>
inline void BoundBox3<T>::setFromCornerAndXYZ(T cornerX, T cornerY, T cornerZ, T sizeX, T sizeY, T sizeZ)
{
    mMin.set(cornerX, cornerY, cornerZ);
    mMax.set(cornerX + sizeX, cornerY + sizeY, cornerZ + sizeZ);
}

template <typename T>
inline void BoundBox3<T>::setFromCornerAndXYZ(const Vector3& corner, T sizeX, T sizeY, T sizeZ)
{
    setFromCornerAndXYZ(corner.x, corner.y, corner.z, sizeX, sizeY, sizeZ);
}

template <typename T>
inline void BoundBox3<T>::setFromCenterAndXYZ(T centerX, T centerY, T centerZ, T sizeX, T sizeY, T sizeZ)
{
    T halfWidth = sizeX / 2.0f;
    T halfHeight = sizeY / 2.0f;
    T halfDepth = sizeZ / 2.0f;

    mMin.set(centerX - halfWidth, centerY - halfHeight, centerZ - halfDepth);
    mMax.set(centerX + halfWidth, centerY + halfHeight, centerZ + halfDepth);
}

template <typename T>
inline void BoundBox3<T>::setFromCenterAndXYZ(const Vector3& center, T sizeX, T sizeY, T sizeZ)
{
    setFromCenterAndXYZ(center.x, center.y, center.z, sizeX, sizeY, sizeZ);
}

template <typename T>
Vector3<T> BoundBox3<T>::getCenter() const
{
    return Vector3((mMin.x + mMax.x) / 2.0f, (mMin.y + mMax.y) / 2.0f, (mMin.z + mMax.z) / 2.0f);
}

template <typename T>
void BoundBox3<T>::getCenter(Vector3* p) const
{
    p->set((mMin.x + mMax.x) / 2.0f, (mMin.y + mMax.y) / 2.0f, (mMin.z + mMax.z) / 2.0f);
}

template <typename T>
bool BoundBox3<T>::isInside(const Vector3& p) const
{
    return mMin.x <= p.x && p.x <= mMax.x &&
           mMin.y <= p.y && p.y <= mMax.y &&
           mMin.z <= p.z && p.z <= mMax.z;
}

template <typename T>
inline void BoundBox3<T>::offset(T dx, T dy, T dz)
{
    mMin.x += dx;
    mMin.y += dy;
    mMin.z += dz;
    mMax.x += dx;
    mMax.y += dy;
    mMax.z += dz;
}

template <typename T>
inline void BoundBox3<T>::offset(const Vector3& dv)
{
    offset(dv.x, dv.y, dv.z);
}

template <typename T>
inline void BoundBox3<T>::scaleX(T sx)
{
    T hx = getSizeX() * (sx / 2.0f);
    T ox = (mMin.x + mMax.x) / 2.0f;

    mMin.x = ox - hx;
    mMax.x = ox + hx;
}

template <typename T>
inline void BoundBox3<T>::scaleY(T sy)
{
    T hy = getSizeY() * (sy / 2.0f);
    T oy = (mMin.y + mMax.y) / 2.0f;

    mMin.y = oy - hy;
    mMax.y = oy + hy;
}

template <typename T>
inline void BoundBox3<T>::scaleZ(T sz)
{
    T hz = getSizeZ() * (sz / 2.0f);
    T oz = (mMin.z + mMax.z) / 2.0f;

    mMin.z = oz - hz;
    mMax.z = oz + hz;
}

} // namespace sead
