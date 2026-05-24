#pragma once

#include <filedevice/posix/seadPosixFileDevicePosix.h>

namespace sead {

class PosixContentFileDevice : public PosixFileDevice
{
    SEAD_RTTI_OVERRIDE(PosixContentFileDevice, PosixFileDevice);

public:
    PosixContentFileDevice();
};

} // namespace sead
