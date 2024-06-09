#pragma once

#include <math/seadMathCalcCommon.h>
#include <math/seadVector.h>

namespace sead {

// TODO
template <typename T>
class BoundBox2
{
public:
    using Vector2 = sead::Vector2<T>;

public:
    BoundBox2()
    {
        setUndef();
    }

    BoundBox2(T x0, T y0, T x1, T y1)
    {
        set(x0, y0, x1, y1);
    }

    void set(T x0, T y0, T x1, T y1)
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

    void setUndef()
    {
        mMin.set(MathCalcCommon<T>::maxNumber(), MathCalcCommon<T>::maxNumber());
        mMax.set(MathCalcCommon<T>::minNumber(), MathCalcCommon<T>::minNumber());
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
        return (mMax.x - mMin.x) / 2.0f;
    }

    T getHalfSizeY() const
    {
        return (mMax.y - mMin.y) / 2.0f;
    }

    Vector2& getMin()
    {
        return mMin;
    }

    const Vector2& getMin() const
    {
        return mMin;
    }

    Vector2& getMax()
    {
        return mMax;
    }

    const Vector2& getMax() const
    {
        return mMax;
    }

    void setMin(const Vector2& min)
    {
        mMin = min;
    }

    void setMax(const Vector2& max)
    {
        mMax = max;
    }

    Vector2 getCenter() const;

    void getCenter(Vector2* p) const
    {
        p->set((mMin.x + mMax.x) / 2.0f, (mMin.y + mMax.y) / 2.0f);
    }

    bool isUndef() const
    {
        return mMin.x > mMax.x || mMin.y > mMax.y;
    }

    bool isInside(const Vector2& p) const
    {
        return mMin.x <= p.x && p.x <= mMax.x && mMin.y <= p.y && p.y <= mMax.y;
    }

private:
    Vector2 mMin;
    Vector2 mMax;
};

using BoundBox2f = BoundBox2<f32>;

} // namespace sead
