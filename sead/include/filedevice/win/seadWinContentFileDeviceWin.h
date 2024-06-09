#pragma once

#include <filedevice/win/seadWinFileDeviceWin.h>

namespace sead {

class WinContentFileDevice : public WinFileDevice
{
    SEAD_RTTI_OVERRIDE(WinContentFileDevice, WinFileDevice);

public:
    WinContentFileDevice();
};

} // namespace sead
