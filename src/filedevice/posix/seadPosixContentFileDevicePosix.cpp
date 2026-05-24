#include <filedevice/posix/seadPosixContentFileDevicePosix.h>

namespace sead {

PosixContentFileDevice::PosixContentFileDevice()
    : PosixFileDevice("content", "content")
{
}

} // namespace sead
