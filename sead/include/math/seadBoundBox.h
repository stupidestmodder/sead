#pragma once

#include <math/seadVector.h>

namespace sead {

// TODO
template <typename T>
class BoundBox2
{
public:
    using Vector2 = sead::Vector2<T>;

public:
    BoundBox2();

private:
    Vector2 mMin;
    Vector2 mMax;
};

using BoundBox2f = BoundBox2<f32>;

} // namespace sead
