#pragma once

#include <filedevice/posix/seadPosixFileDevicePosix.h>

namespace sead {

class PosixNativeFileDevice : public PosixFileDevice
{
    SEAD_RTTI_OVERRIDE(PosixNativeFileDevice, PosixFileDevice);

public:
    PosixNativeFileDevice();

protected:
    void doResolvePath_(BufferedSafeString* out, const SafeString& path) const override;
};

} // namespace sead
