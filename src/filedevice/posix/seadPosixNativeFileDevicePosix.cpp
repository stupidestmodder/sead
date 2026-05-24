#include <filedevice/posix/seadPosixNativeFileDevicePosix.h>

namespace sead {

PosixNativeFileDevice::PosixNativeFileDevice()
    : PosixFileDevice("native", "")
{
}

void PosixNativeFileDevice::doResolvePath_(BufferedSafeString* out, const SafeString& path) const
{
    // TODO: ?
    // CHAR exPath[512];
    // ExpandEnvironmentStringsA(path.cstr(), exPath, 512);

    out->copy(path.cstr());
}

} // namespace sead
