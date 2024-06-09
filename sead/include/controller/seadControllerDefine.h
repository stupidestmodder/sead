#pragma once

#include <basis/seadTypes.h>

namespace sead {

class ControllerDefine
{
public:
    enum class DeviceId
    {
        eNull = 0,
        eKeyboardMouse,
        eCtrHid,
        eWinJoyPad,
        eRvlWPad,
        eWinWPad,
        eCafeDebugPad,
        eCafeWPad,
        eCafeVPad,
        eAdrInput,
        eInputCapture,
        eNinGenericPad,
        eUserDefine
    };

    enum class ControllerId
    {
        eNull = 0,
        eWin,
        eCtr,
        eCtrDebug,
        eWiiRemote,
        eCafeDebug,
        eWinDRC,
        eCafeRemote,
        eCafeDRC,
        eMerge,
        eAdrVirtual,
        eInputCapture,
        eUserDefine
    };

    enum class AddonId
    {
        eNull = 0,
        eAccelerometer,
        ePatternRumble,
        eUserDefine
    };
};

} // namespace sead
