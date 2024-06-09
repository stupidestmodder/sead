#include <filedevice/win/seadWinNativeFileDeviceWin.h>

namespace sead {

WinNativeFileDevice::WinNativeFileDevice()
    : WinFileDevice("native", "")
{
}

void WinNativeFileDevice::doResolvePath_(BufferedSafeString* out, const SafeString& path) const
{
    CHAR exPath[512];
    ExpandEnvironmentStringsA(path.cstr(), exPath, 512);

    out->copy(exPath);
}

} // namespace sead
