#pragma once

#include <math/seadMathCalcCommon.h>
#include <math/seadVector.h>

namespace sead {

template <typename T>
class BoundBox2
{
public:
    using Vector2 = Vector2<T>;

public:
    BoundBox2()
    {
        setUndef();
    }

    BoundBox2(T x0, T y0, T x1, T y1)
    {
        set(x0, y0, x1, y1);
    }

    BoundBox2(const Vector2& min, const Vector2& max)
    {
        set(min, max);
    }

    bool isUndef() const;

    void set(T x0, T y0, T x1, T y1);
    void set(const Vector2& min, const Vector2& max);
    void setUndef();
    void setFromCornerAndXY(T cornerX, T cornerY, T sizeX, T sizeY);
    void setFromCornerAndXY(const Vector2& corner, T sizeX, T sizeY);
    void setFromCenterAndXY(T centerX, T centerY, T sizeX, T sizeY);
    void setFromCenterAndXY(const Vector2& center, T sizeX, T sizeY); //? This one is implemeted here but let's not.

    void setMin(const Vector2& min)
    {
        mMin = min;
    }

    void setMax(const Vector2& max)
    {
        mMax = max;
    }

    T getSizeX() const
    {
        return mMax.x - mMin.x;
    }

    T getSizeY() const
    {
        return mMax.y - mMin.y;
    }

    T getHalfSizeX() const
    {
        return getSizeX() / 2.0f;
    }

    T getHalfSizeY() const
    {
        return getSizeY() / 2.0f;
    }

    const Vector2& getMin() const
    {
        return mMin;
    }

    Vector2& getMin()
    {
        return mMin;
    }

    const Vector2& getMax() const
    {
        return mMax;
    }

    Vector2& getMax()
    {
        return mMax;
    }

    Vector2 getTL() const
    {
        return mMin;
    }

    Vector2 getTR() const
    {
        return Vector2(mMax.x, mMin.y);
    }

    Vector2 getBL() const
    {
        return Vector2(mMin.x, mMax.y);
    }

    Vector2 getBR() const
    {
        return mMax;
    }

    Vector2 getCenter() const;
    void getCenter(Vector2* p) const;

    bool isInside(const Vector2& p) const;

    void offset(T dx, T dy);
    void offset(const Vector2& dv);
    void scaleX(T sx);
    void scaleY(T sy);

private:
    Vector2 mMin;
    Vector2 mMax;
};

template <typename T>
class BoundBox3
{
public:
    using Vector3 = Vector3<T>;

public:
    BoundBox3()
    {
        setUndef();
    }

    BoundBox3(T x0, T y0, T z0, T x1, T y1, T z1)
    {
        set(x0, y0, z0, x1, y1, z1);
    }

    BoundBox3(const Vector3& min, const Vector3& max)
    {
        set(min, max);
    }

    bool isUndef() const;

    void set(T x0, T y0, T z0, T x1, T y1, T z1);
    void set(const Vector3& min, const Vector3& max);
    void setUndef();
    void setFromCornerAndXYZ(T cornerX, T cornerY, T cornerZ, T sizeX, T sizeY, T sizeZ);
    void setFromCornerAndXYZ(const Vector3& corner, T sizeX, T sizeY, T sizeZ);
    void setFromCenterAndXYZ(T centerX, T centerY, T centerZ, T sizeX, T sizeY, T sizeZ);
    void setFromCenterAndXYZ(const Vector3& center, T sizeX, T sizeY, T sizeZ);

    void setMin(const Vector3& min)
    {
        mMin = min;
    }

    void setMax(const Vector3& max)
    {
        mMax = max;
    }

    T getSizeX() const
    {
        return mMax.x - mMin.x;
    }

    T getSizeY() const
    {
        return mMax.y - mMin.y;
    }

    T getSizeZ() const
    {
        return mMax.z - mMin.z;
    }

    T getHalfSizeX() const
    {
        return getSizeX() / 2.0f;
    }

    T getHalfSizeY() const
    {
        return getSizeY() / 2.0f;
    }

    T getHalfSizeZ() const
    {
        return getSizeZ() / 2.0f;
    }

    const Vector3& getMin() const
    {
        return mMin;
    }

    Vector3& getMin()
    {
        return mMin;
    }

    const Vector3& getMax() const
    {
        return mMax;
    }

    Vector3& getMax()
    {
        return mMax;
    }

    Vector3 getCenter() const;
    void getCenter(Vector3* p) const;

    bool isInside(const Vector3& p) const;

    void offset(T dx, T dy, T dz);
    void offset(const Vector3& dv);
    void scaleX(T sx);
    void scaleY(T sy);
    void scaleZ(T sz);

private:
    Vector3 mMin;
    Vector3 mMax;
};

using BoundBox2f = BoundBox2<f32>;
using BoundBox2d = BoundBox2<f64>;
using BoundBox2i = BoundBox2<s32>;
using BoundBox2u = BoundBox2<u32>;

using BoundBox3f = BoundBox3<f32>;
using BoundBox3d = BoundBox3<f64>;
using BoundBox3i = BoundBox3<s32>;
using BoundBox3u = BoundBox3<u32>;

} // namespace sead

#include <math/seadBoundBox.hpp>
