#pragma once

#include <math/seadQuat.h>
#include <math/seadQuatCalcCommon.h>

namespace sead {

template <typename T>
inline bool Quat<T>::makeVectorRotation(const Vec3& from, const Vec3& to)
{
    return QuatCalcCommon<T>::makeVectorRotation(*this, from, to);
}

} // namespace sead
