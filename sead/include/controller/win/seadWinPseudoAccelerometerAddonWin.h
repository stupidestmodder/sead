#pragma once

#include <controller/seadAccelerometerAddon.h>

namespace sead {

class WinPseudoAccelerometerAddon : public AccelerometerAddon
{
    SEAD_RTTI_OVERRIDE(WinPseudoAccelerometerAddon, AccelerometerAddon);

public:
    WinPseudoAccelerometerAddon(Controller* controller);

    bool calc() override;

protected:
    f32 mUnkFloat;
    u8 mUnkButton1;
    u8 mUnkButton2;
    Vector2i mUnkVec1;
    Vector2i mUnkVec2;
};

} // namespace sead
