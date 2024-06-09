#pragma once

#include <controller/seadController.h>
#include <controller/seadControllerWrapperBase.h>

namespace sead {

class ControllerWrapper : public ControllerWrapperBase
{
    SEAD_RTTI_OVERRIDE(ControllerWrapper, ControllerWrapperBase);

public:
    static const u8 cPadConfigDefault[Controller::PadIdx::eMax];

public:
    ControllerWrapper();

    virtual void calc(u32 prevHold, bool prevPointerOn);

    void setPadConfig(s32 padbitMax, const u8* padConfig, bool enableStickcrossEmulation);

protected:
    u32 createPadMaskFromControllerPadMask_(u32 controllerMask) const;

protected:
    u8 mPadConfig[PadIdx::eMaxBase];
};

} // namespace sead
