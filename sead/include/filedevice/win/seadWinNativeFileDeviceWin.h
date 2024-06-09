#pragma once

#include <filedevice/win/seadWinFileDeviceWin.h>

namespace sead {

class WinNativeFileDevice : public WinFileDevice
{
    SEAD_RTTI_OVERRIDE(WinNativeFileDevice, WinFileDevice);

public:
    WinNativeFileDevice();

protected:
    void doResolvePath_(BufferedSafeString* out, const SafeString& path) const override;
};

} // namespace sead
