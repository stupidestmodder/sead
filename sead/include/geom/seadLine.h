#pragma once

#include <math/seadVector.h>

namespace sead {

// TODO
template <typename T>
class Ray
{
public:
    Ray();

    void setBy2Points(const T& p0, const T& p1);

    void setPos(const T& p);
    void setDir(const T& d, bool isNormalized);

private:
    T mP;
    T mD;
};

using Ray2f = Ray<Vector2f>;
using Ray3f = Ray<Vector3f>;

} // namespace sead
