#pragma once

#include <controller/seadControllerAddon.h>
#include <math/seadVector.h>

namespace sead {

class AccelerometerAddon : public ControllerAddon
{
    SEAD_RTTI_OVERRIDE(AccelerometerAddon, ControllerAddon);

public:
    explicit AccelerometerAddon(Controller* controller)
        : ControllerAddon(controller)
        , mIsEnable(false)
        , mAcceleration(0.0f, 0.0f, 0.0f)
    {
        mId = ControllerDefine::AddonId::eAccelerometer;
    }

    bool isEnable() const { return mIsEnable; }
    const Vector3f& getAcceleration() const { return mAcceleration; }

protected:
    bool mIsEnable;
    Vector3f mAcceleration;
};

} // namespace sead
